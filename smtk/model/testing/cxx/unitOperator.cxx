//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/AutoInit.h"

#include "smtk/io/AttributeReader.h"
#include "smtk/io/Logger.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/IntItem.h"

#include "smtk/model/DefaultSession.h"
#include "smtk/model/Manager.h"
#include "smtk/model/Model.h"
#include "smtk/model/Operator.h"
#include "smtk/model/Session.h"
#include "smtk/model/SessionRef.h"

#include "smtk/common/testing/cxx/helpers.h"
#include "smtk/model/testing/cxx/helpers.h"

#include "smtk/Options.h"

// Encoded XML describing the operator classes below.
// These are generated by CMake from unitXXXOperator.sbt.
#include "unitOutcomeOperator_xml.h"

using namespace smtk::model;
using smtk::attribute::IntItem;

template <typename T>
void printVec(const std::vector<T>& v, const char* typeName, char sep = '\0')
{
  if (v.empty())
    return;
  std::cout << " " << typeName << "(" << v.size() << "): [";
  std::cout << " " << v[0];
  if (sep)
    for (typename std::vector<T>::size_type i = 1; i < v.size(); ++i)
      std::cout << sep << " " << v[i];
  else
    for (typename std::vector<T>::size_type i = 1; i < v.size(); ++i)
      std::cout << " " << v[i];
  std::cout << " ]";
}

int WillOperateWatcher(OperatorEventType event, const Operator& op, void* user)
{
  test(event == WILL_OPERATE, "WillOperate callback invoked with bad event type");
  int shouldCancel = *reinterpret_cast<int*>(user);
  std::cout << "Operator " << op.name() << " about to run: " << (shouldCancel ? "will" : "will not")
            << " request cancellation.\n";
  return shouldCancel;
}

int DidOperateWatcher(
  OperatorEventType event, const Operator& op, OperatorResult result, void* user)
{
  test(event == DID_OPERATE, "DidOperate callback invoked with bad event type");
  IntItem::Ptr outcome = result->findInt("outcome");
  std::cout << "Operator " << op.name() << " did operate"
            << " (outcome = \"" << outcomeAsString(outcome->value()) << "\")\n";
  // increment the number of times the parameter was modified.
  (*reinterpret_cast<int*>(user))++;
  return 0;
}

void testSessionList(Manager::Ptr manager)
{
  std::cout << "Available sessions\n";
  StringList sessions = manager->sessionTypeNames();
  for (StringList::iterator it = sessions.begin(); it != sessions.end(); ++it)
    std::cout << "  " << *it << "\n";
  std::cout << "\n";
}

class TestOutcomeOperator : public Operator
{
public:
  smtkTypeMacro(TestOutcomeOperator);
  smtkCreateMacro(TestOutcomeOperator);
  smtkSharedFromThisMacro(Operator);
  smtkDeclareModelOperator();

  TestOutcomeOperator()
    : m_able(false) // fail operation until told otherwise
  {
  }
  bool ableToOperate() override
  {
    this->ensureSpecification();
    return
      // Force failure?
      this->m_able && this->m_specification && this->m_specification->isValid();
  }
  bool m_able; // Used to force UNABLE_TO_OPERATE result.

protected:
  OperatorResult operateInternal() override
  {
    return this->createResult(this->specification()->findInt("shouldSucceed")->value()
        ? OPERATION_SUCCEEDED
        : OPERATION_FAILED);
  }
};

// Implementation corresponding to smtkDeclareModelOperator() above.

/* Do not invoke smtkImplementsModelOperator so that we can test
 * post-session-construction registration of operators.
 */
/*
smtkImplementsModelOperator(
  ,
  TestOutcomeOperator,
  OutcomeOp,
  "outcome test",
  unitOutcomeOperator_xml,
  smtk::model::DefaultSession);
 */

smtk::model::OperatorPtr TestOutcomeOperator::baseCreate()
{
  return TestOutcomeOperator::create();
}

std::string TestOutcomeOperator::operatorName("outcome test");
std::string TestOutcomeOperator::className() const
{
  return "TestOutcomeOperator";
}

void testExPostFactoOperatorRegistration(Manager::Ptr manager)
{
  // Add a default session.
  smtk::model::SessionRef defSess = manager->createSession("native");

  typedef std::vector<smtk::attribute::DefinitionPtr> OpListType;
  // Add operator descriptions to the default session of our manager.
  smtk::model::SessionPtr session = defSess.session();
  smtk::attribute::DefinitionPtr opBase = session->operatorCollection()->findDefinition("operator");
  OpListType origOpList;
  session->operatorCollection()->derivedDefinitions(opBase, origOpList);

  // Register the operator
  session->registerOperator(
    TestOutcomeOperator::operatorName, unitOutcomeOperator_xml, &TestOutcomeOperator::baseCreate);

  // Now enumerate attribute definitions that inherit "operator".
  OpListType opList;
  session->operatorCollection()->derivedDefinitions(opBase, opList);
  std::cout << "Imported XML for operators:\n";
  for (OpListType::iterator it = opList.begin(); it != opList.end(); ++it)
  {
    std::cout << "  \"" << (*it)->type() << "\"\n";
  }
  std::cout << "\n";

  if (opList.size() != origOpList.size() + 1)
  {
    std::ostringstream err;
    err << "Error: Expected " << (origOpList.size() + 1) << " operator(s), found " << opList.size()
        << "\n";
    std::cerr << err.str();
    throw err.str();
  }
}

void testOperatorOutcomes(Manager::Ptr manager)
{
  TestOutcomeOperator::Ptr op = smtk::dynamic_pointer_cast<TestOutcomeOperator>(
    manager->sessions().begin()->op("outcome test"));

  int shouldCancel = 1;
  int numberOfFailedOperations = 0;
  op->observe(WILL_OPERATE, WillOperateWatcher, &shouldCancel);
  op->observe(DID_OPERATE, DidOperateWatcher, &numberOfFailedOperations);

  // Verify that ableToOperate() is called and complains:
  test(op->operate()->findInt("outcome")->value() == UNABLE_TO_OPERATE,
    "Should have been unable to operate.");
  std::cout << "Operator " << op->name() << " should be unable to operate"
            << " (outcome = \"" << outcomeAsString(op->operate()->findInt("outcome")->value())
            << "\""
            << ").\n--\n";

  // Verify that the WILL_OPERATE observer is called and cancels the operation:
  op->m_able = true;
  test(op->operate()->findInt("outcome")->value() == OPERATION_CANCELED,
    "Operation should have been canceled.");
  std::cout << "--\n";

  // Verify that the operation fails when "shouldSucceed" parameter is zero.
  shouldCancel = 0;
  test(op->operate()->findInt("outcome")->value() == OPERATION_FAILED,
    "Operation should have failed.");
  std::cout << "--\n";

  // Verify that removing observer bypasses cancellation.
  op->unobserve(WILL_OPERATE, WillOperateWatcher, &shouldCancel);
  shouldCancel = 1; // Force cancellation if observer wasn't removed.

  // Verify that the parameter-change callback is invoked.
  op->specification()->findInt("shouldSucceed")->setValue(1);

  // Verify that the operation succeeds when "shouldSucceed" parameter is non-zero.
  test(op->operate()->findInt("outcome")->value() == OPERATION_SUCCEEDED,
    "Operation should have succeeded.");
  std::cout << "--\n";

  // Now test values passed to callbacks:
  test(numberOfFailedOperations > 0, "Expected more operator failures.");

  op->unobserve(DID_OPERATE, DidOperateWatcher, &numberOfFailedOperations);
}

void testSessionAssociation(Manager::Ptr manager)
{
  // Test that operators added by previous tests still exist
  Model model = manager->addModel(3, 3, "Model Airplane");
  model.setSession(*manager->sessions().begin());
  StringList modelOpNames = model.operatorNames();
  test(std::find(modelOpNames.begin(), modelOpNames.end(), "outcome test") != modelOpNames.end(),
    "Expected \"outcome test\" operator defined for the test model.");

  // Test op(name) method
  Operator::Ptr op = model.op("outcome test");
  test(op ? 1 : 0, "Model::op(\"outcome test\") returned a \"null\" shared pointer.");

  // Test Operator->Session association
  test(
    op->session() == manager->sessions().begin()->session(), "Bad session reported by operator.");

  // Test Operator->Manager association
  test(op->manager() == manager, "Bad manager reported by operator.");

  // Test operatorNames()
  StringList opNames = model.session().operatorNames();
  std::cout << "Printing";
  printVec(opNames, "operator names", ',');
  std::cout << "\n";
}

int main()
{
  int status = 0;

  Manager::Ptr manager = Manager::create();

  try
  {

    testSessionList(manager);
    testExPostFactoOperatorRegistration(manager);
    testOperatorOutcomes(manager);
    testSessionAssociation(manager);
  }
  catch (const std::string& msg)
  {
    (void)msg; // Ignore the message; it's already been printed.
    std::cerr << "Exiting...\n";
    status = -1;
  }

  return status;
}
