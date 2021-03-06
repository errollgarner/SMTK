//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#ifndef __smtk_session_discrete_Session_h
#define __smtk_session_discrete_Session_h

#include "smtk/PublicPointerDefs.h"
#include "smtk/bridge/discrete/Exports.h"
#include "smtk/model/Session.h"

#include "smtk/common/UUID.h"
#include "smtk/common/UUIDGenerator.h"

#include "vtkSmartPointer.h"
#include "vtkWeakPointer.h"

#include <map>

class vtkAbstractArray;
class vtkDiscreteModel;
class vtkDiscreteModelEdge;
class vtkDiscreteModelEntityGroup;
class vtkDiscreteModelFace;
class vtkDiscreteModelGeometricEntity;
class vtkDiscreteModelRegion;
class vtkDiscreteModelVertex;
class vtkDiscreteModelWrapper;
class vtkInformation;
class vtkModel;
class vtkModelEdge;
class vtkModelEdgeUse;
class vtkModel;
class vtkModelFace;
class vtkModelFaceUse;
class vtkModelGeometricEntity;
class vtkModelItem;
class vtkModelItemIterator;
class vtkModelLoopUse;
class vtkModelMaterial;
class vtkModelRegion;
class vtkModelShellUse;
class vtkModelVertex;
class vtkModelVertexUse;
class vtkUnsignedIntArray;

namespace smtk
{
namespace model
{
class Group;
}
}

namespace smtk
{
namespace bridge
{
namespace discrete
{

class ArrangementHelper;
class vtkItemWatcherCommand;

/**\brief A class that handles translation between CMB and SMTK instances.
  *
  * How it works:
  * (1) A Session entity and ReadOperator are created. The read operator is called
  *     to load a CMB model. Internally, a CMBModelReadOperator is used to load
  *     a vtkDiscreteModel (placed inside a vtkDiscreteModelWrapper by the
  *     CMB operator). The Session instance is associated to the model wrapper
  *     in the m_modelsToSessions variable.
  * (2) The ReadOperator calls trackModel with the vtkDiscreteModelWrapper
  *     obtained using CMB's (not SMTK's) "read" operator. Since
  *     vtkDiscreteModelWrapper is a subclass of vtkObject,
  *     we can keep it from being destroyed by holding a smart-pointer to
  *     it (in m_modelIdsToRefs and m_modelRefsToIds).
  * (2) The model and, upon demand, entities contained in the model
  *     are assigned UUIDs if not present already. The UUIDs are kept
  *     in the vtkInformation object every vtkModelItem owns (Properties).
  * (3) An SMTK WriteOperator will accept a list of UUIDs for vtkModel
  *     instances. For each top-level vtkDiscreteModelWrapper in the list,
  *     it calls CMB's "write" operator on the model.
  */
class SMTKDISCRETESESSION_EXPORT Session : public smtk::model::Session
{
public:
  smtkTypeMacro(Session);
  smtkSuperclassMacro(smtk::model::Session);
  smtkCreateMacro(smtk::model::Session);
  smtkSharedFromThisMacro(smtk::model::Session);
  smtkDeclareModelingKernel();

  typedef smtk::model::SessionInfoBits SessionInfoBits;
  virtual ~Session();

  SessionInfoBits allSupportedInformation() const override;

  smtk::model::EntityRef addCMBEntityToManager(
    const smtk::common::UUID& entity, smtk::model::ManagerPtr storage, int relDepth = 1);

  void assignUUIDs(const std::vector<vtkModelItem*>& ents, vtkAbstractArray* uuidArray);

  vtkUnsignedIntArray* retrieveUUIDs(
    vtkDiscreteModel* model, const std::vector<vtkModelItem*>& ents);

  int ExportEntitiesToFileOfNameAndType(const std::vector<smtk::model::EntityRef>& entities,
    const std::string& filename, const std::string& filetype);

  vtkDiscreteModelWrapper* findModelEntity(const smtk::common::UUID& uid) const;

  std::string defaultFileExtension(const smtk::model::Model& model) const override;

protected:
  friend class vtkItemWatcherCommand;
  friend class MergeOperator;
  friend class ReadOperator;
  friend class SplitFaceOperator;
  friend class ImportOperator;
  friend class EntityGroupOperator;
  friend class GrowOperator;
  friend class CreateEdgesOperator;
  friend class WriteOperator;
  friend class RemoveModel;
  friend class ArrangementHelper;
  friend class EdgeOperator;
  friend class Operator;

  Session();

  smtk::model::Entity* addEntityRecord(const smtk::model::EntityRef& entRef) override;
  smtk::model::ArrangementHelper* createArrangementHelper() override;
  int findOrAddCellAdjacencies(const smtk::model::CellEntity& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddCellUses(const smtk::model::CellEntity& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddOwningCell(const smtk::model::UseEntity& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddShellAdjacencies(const smtk::model::UseEntity& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddUseAdjacencies(const smtk::model::ShellEntity& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddGroupOwner(const smtk::model::Group& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddFreeCells(const smtk::model::Model& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddRelatedModels(const smtk::model::Model& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddPrototype(const smtk::model::Instance& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddRelatedModels(const smtk::model::SessionRef& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddRelatedGroups(const smtk::model::EntityRef& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  int findOrAddRelatedInstances(const smtk::model::EntityRef& entRef, SessionInfoBits request,
    smtk::model::ArrangementHelper* helper) override;
  SessionInfoBits findOrAddArrangements(const smtk::model::EntityRef& entRef,
    smtk::model::Entity* entRec, SessionInfoBits flags,
    smtk::model::ArrangementHelper* helper) override;
  SessionInfoBits updateProperties(const smtk::model::EntityRef& entRef,
    smtk::model::Entity* entRec, SessionInfoBits flags,
    smtk::model::ArrangementHelper* helper) override;
  SessionInfoBits updateTessellation(const smtk::model::EntityRef& entRef, SessionInfoBits flags,
    smtk::model::ArrangementHelper* helper) override;

  smtk::common::UUID trackModel(
    vtkDiscreteModelWrapper* mod, const std::string& url, smtk::model::ManagerPtr storage);
  bool assignUUIDToEntity(const smtk::common::UUID& itemId, vtkModelItem* item);
  smtk::common::UUID findOrSetEntityUUID(vtkModelItem* item);
  smtk::common::UUID findOrSetEntityUUID(vtkInformation* itemProperties);
  void untrackEntity(const smtk::common::UUID& itemId);

  //static vtkDiscreteModel* owningModel(vtkModelItem* e);
  vtkModelItem* entityForUUID(const smtk::common::UUID& uid);

  smtk::model::EntityRef addCMBEntityToManager(const smtk::common::UUID& entity,
    vtkModelItem* refEnt, smtk::model::ManagerPtr storage, int relDepth = 1);

  smtk::model::Model addBodyToManager(
    const smtk::common::UUID&, vtkModel*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Group addGroupToManager(const smtk::common::UUID&, vtkDiscreteModelEntityGroup*,
    smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Group addMaterialToManager(
    const smtk::common::UUID&, vtkModelMaterial*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::FaceUse addFaceUseToManager(
    const smtk::common::UUID&, vtkModelFaceUse*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::EdgeUse addEdgeUseToManager(
    const smtk::common::UUID&, vtkModelEdgeUse*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::VertexUse addVertexUseToManager(
    const smtk::common::UUID&, vtkModelVertexUse*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Shell addShellToManager(
    const smtk::common::UUID&, vtkModelShellUse*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Loop addLoopToManager(
    const smtk::common::UUID&, vtkModelLoopUse*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Volume addVolumeToManager(
    const smtk::common::UUID&, vtkModelRegion*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Face addFaceToManager(
    const smtk::common::UUID&, vtkModelFace*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Edge addEdgeToManager(
    const smtk::common::UUID&, vtkModelEdge*, smtk::model::ManagerPtr, int relDepth = 1);
  smtk::model::Vertex addVertexToManager(
    const smtk::common::UUID&, vtkModelVertex*, smtk::model::ManagerPtr, int relDepth = 1);

  template <class P, typename H>
  void addEntities(P& parent, vtkModelItemIterator* it, const H& method, int relDepth);

  template <class P, typename C, typename H>
  void addEntityArray(P& parent, C& childContainer, const H& method, int relDepth);

  void addEntity(const smtk::model::EntityRef& parent, vtkModelItem* child,
    smtk::model::ArrangementKind k, ArrangementHelper* helper, int sense = -1,
    smtk::model::Orientation orientation = smtk::model::UNDEFINED, int iterpos = 0);

  void addEntity(vtkModelItem* parent, const smtk::model::EntityRef& child,
    smtk::model::ArrangementKind k, ArrangementHelper* helper, int sense = -1,
    smtk::model::Orientation orientation = smtk::model::UNDEFINED, int iterpos = 0);

  int addEntities(const smtk::model::EntityRef& parent, vtkModelItemIterator* it,
    smtk::model::ArrangementKind k, ArrangementHelper* helper);
  template <typename T>
  int addEntities(const smtk::model::EntityRef& parent, vtkModelItemIterator* it,
    smtk::model::ArrangementKind k, ArrangementHelper* helper, T& senseLookup);

  int addEntities(vtkModelItemIterator* it, const smtk::model::EntityRef& parent,
    smtk::model::ArrangementKind k, ArrangementHelper* helper);

  template <typename T>
  int addEntities(vtkModelItemIterator* it, const smtk::model::EntityRef& parent,
    smtk::model::ArrangementKind k, ArrangementHelper* helper, T& senseLookup);

  bool addTessellation(const smtk::model::EntityRef& cellOut, vtkModelGeometricEntity* cellIn);
  bool addProperties(
    smtk::model::EntityRef& cellOut, vtkModelItem* cellIn, smtk::model::BitFlags props = 0xff);

  // This will remove Model from smtk manager and vtkDiscreteModelWrapper form kernel
  bool removeModelEntity(const smtk::model::EntityRef& entity);

  void retranscribeModel(const smtk::model::Model& inModel);

  vtkItemWatcherCommand* m_itemWatcher;
  smtk::common::UUIDGenerator m_idGenerator;
  std::map<smtk::common::UUID, vtkWeakPointer<vtkModelItem> > m_itemsToRefs;

  /// Track which models are tracked by which sessions.
  std::map<vtkDiscreteModel*, WeakPtr> m_modelsToSessions;
  /// Associate UUIDs to vtkDiscreteModelWrapper instances.
  std::map<smtk::common::UUID, vtkSmartPointer<vtkDiscreteModelWrapper> > m_modelIdsToRefs;
  /// Associate vtkDiscreteModelWrapper instances to UUIDs.
  std::map<vtkSmartPointer<vtkDiscreteModelWrapper>, smtk::common::UUID> m_modelRefsToIds;
};

} // namespace discrete
} // namespace bridge

} // namespace smtk

#endif // __smtk_session_discrete_Session_h
