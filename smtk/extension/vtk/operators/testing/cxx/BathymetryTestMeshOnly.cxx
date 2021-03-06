//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#include "smtk/bridge/discrete/Session.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/DoubleItem.h"
#include "smtk/attribute/FileItem.h"
#include "smtk/attribute/IntItem.h"
#include "smtk/attribute/MeshItem.h"
#include "smtk/attribute/ModelEntityItem.h"
#include "smtk/attribute/StringItem.h"
#include "smtk/attribute/VoidItem.h"

#include "smtk/io/ExportMesh.h"

#include "smtk/model/EntityPhrase.h"
#include "smtk/model/EntityRef.h"
#include "smtk/model/Face.h"
#include "smtk/model/Group.h"
#include "smtk/model/Manager.h"
#include "smtk/model/Model.h"
#include "smtk/model/Operator.h"
#include "smtk/model/SimpleModelSubphrases.h"
#include "smtk/model/Tessellation.h"

#include "smtk/mesh/Collection.h"
#include "smtk/mesh/Manager.h"
#include "smtk/mesh/testing/cxx/helpers.h"

#include "smtk/extension/vtk/source/vtkMeshMultiBlockSource.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkCompositePolyDataMapper.h"
#include "vtkDataSetAttributes.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkXMLMultiBlockDataWriter.h"

#include <cstdlib>

using namespace smtk::model;

int main(int argc, char* argv[])
{
  if (argc < 3)
    return 1;
  smtk::model::ManagerPtr manager = smtk::model::Manager::create();

  std::cout << "Available sessions\n";
  StringList sessions = manager->sessionTypeNames();
  for (StringList::iterator it = sessions.begin(); it != sessions.end(); ++it)
    std::cout << "  " << *it << "\n";
  std::cout << "\n";

  smtk::bridge::discrete::Session::Ptr session = smtk::bridge::discrete::Session::create();
  manager->registerSession(session);

  std::cout << "Available cmb operators in discrete session\n";
  StringList opnames = session->operatorNames();
  for (StringList::iterator it = opnames.begin(); it != opnames.end(); ++it)
    std::cout << "  " << *it << "\n";
  std::cout << "\n";

  // read the data
  smtk::model::OperatorPtr readOp = session->op("import");
  if (!readOp)
  {
    std::cerr << "No import operator\n";
    return 1;
  }

  readOp->specification()->findFile("filename")->setValue(std::string(argv[1]));
  std::cout << "Importing " << argv[1] << "\n";
  smtk::model::OperatorResult opresult = readOp->operate();
  if (opresult->findInt("outcome")->value() != smtk::model::OPERATION_SUCCEEDED)
  {
    std::cerr << "Read operator failed\n";
    return 1;
  }
  // assign model value
  smtk::model::Model model2dm = opresult->findModelEntity("created")->value();
  manager->assignDefaultNames(); // should force transcription of every entity, but doesn't yet.

  smtk::attribute::ModelEntityItemPtr meshedFaceItem = opresult->findModelEntity("mesh_created");

  if (!meshedFaceItem || !meshedFaceItem->isValid())
  {
    std::cerr << "No associated mesh!\n";
    return 1;
  }

  const smtk::model::Face& meshedFace = meshedFaceItem->value();
  auto associatedCollections = manager->meshes()->associatedCollections(meshedFace);
  smtk::mesh::CollectionPtr collection = associatedCollections[0];
  smtk::mesh::MeshSet mesh = collection->meshes();

  if (!model2dm.isValid())
  {
    std::cerr << "Reading model file failed!\n";
    return 1;
  }

  // add auxiliary geometry
  smtk::model::OperatorPtr aux_geOp = session->op("add auxiliary geometry");
  std::cout << "The url for auxiliary geometry is: " << argv[2] << std::endl;
  aux_geOp->specification()->findFile("url")->setValue(std::string(argv[2]));
  aux_geOp->associateEntity(model2dm);
  smtk::model::OperatorResult aux_geOpresult = aux_geOp->operate();
  if (aux_geOpresult->findInt("outcome")->value() != smtk::model::OPERATION_SUCCEEDED)
  {
    std::cerr << "Add auxiliary geometry failed!\n";
    return 1;
  }

  smtk::model::AuxiliaryGeometry auxGo2dm = aux_geOpresult->findModelEntity("created")->value();
  std::cout << "After aux_geo op, the url inside is: " << auxGo2dm.url() << std::endl;
  if (!auxGo2dm.isValid())
  {
    std::cerr << "Auxiliary geometry is not valid!\n";
    return 1;
  }

  // create the bathymetry operator
  std::cout << "Creating apply bathymetry operator\n";
  smtk::model::OperatorPtr bathyOperator = session->op("apply bathymetry");
  if (!bathyOperator)
  {
    std::cerr << "No apply bathymetry!\n";
    return 1;
  }
  // Check the optypeItem value

  bathyOperator->specification()->findString("operation")->setValue("Apply Bathymetry (Mesh Only)");

  // set input values for bathymetry filter
  bathyOperator->specification()->findModelEntity("auxiliary geometry")->setValue(auxGo2dm);

  double radius = 10.;
  if (argc < 4)
  {
    radius = atof(argv[4]);
  }

  bathyOperator->specification()->findDouble("averaging elevation radius")->setValue(radius);
  bathyOperator->specification()->findMesh("mesh")->appendValue(mesh);
  bathyOperator->specification()->findVoid("invert scalars")->setIsEnabled(true);

  smtk::model::OperatorResult bathyResult = bathyOperator->operate();
  if (bathyResult->findInt("outcome")->value() != smtk::model::OPERATION_SUCCEEDED)
  {
    std::cerr << "Apply bathymetry operator failed\n";
    return 1;
  }

  const bool result = smtk::io::exportMesh(std::string(argv[3]), collection);

  return result ? 0 : 1;
}

// This macro ensures the vtk io library is loaded into the executable
smtkComponentInitMacro(smtk_apply_bathymetry_operator)
