//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

// .NAME qtSelectionManager - The selection manager for smtk
// .SECTION Description
// .SECTION See Also

#ifndef __smtk__extension_qtSelectionManager_h
#define __smtk__extension_qtSelectionManager_h

#include "smtk/extension/qt/Exports.h"
#include "smtk/extension/qt/qtItem.h"
#include "smtk/model/EntityTypeBits.h"

#include "smtk/PublicPointerDefs.h"
#include "smtk/common/UUID.h"
#include "smtk/mesh/MeshSet.h"
#include "smtk/model/DescriptivePhrase.h"
#include "smtk/model/EntityRef.h"
#include "smtk/model/SessionRef.h"

namespace smtk
{
namespace extension
{

enum class SelectionModifier
{
  SELECTION_REPLACE_FILTERED = 0,       /* !< Replace all the current selection
                                    and filter it. Example user case: normal
                                    selection from rendering window. */
  SELECTION_REPLACE_UNFILTERED = 1,     /* !< Replace all the current selection
                              and do not filter it. Example user case: selection
                                from model tree and attribute panel. */
  SELECTION_ADDITION_FILTERED = 2,      /* !< Filter the input selection and add it
                              to the current selection. Example user case:
                                addition from rendering window. */
  SELECTION_ADDITION_UNFILTERED = 3,    /* !< Do not filter the input selection
                                    and add it to the current selection.
                          Example user case: addition from operator dialog. */
  SELECTION_SUBTRACTION_FILTERED = 4,   /* !< Filter the input selection and
                                      subtract it from the current selection.
                      Example user case: substraction from rendering window. */
  SELECTION_SUBTRACTION_UNFILTERED = 5, /* !< Do not filter the input
                    selection and sbutract it from the current selection.
                Example user case: subtraction from operator dialog. */
  SELECTION_INQUIRY = 6                 /* !< Use the SelectionModifer defined in
                        qtSelectionManager. Use it when SelectionModifer is
                          decided by user input(Ex. different keybindings).
    Example user case: rendering window selection addition and subtraction. */
};

class SMTKQTEXT_EXPORT qtSelectionManager : public QObject
{
public:
  Q_OBJECT
public:
  qtSelectionManager();
  void setModelManager(smtk::model::ManagerPtr mgrPtr) { this->m_modelMgr = mgrPtr; }
  void getSelectedEntities(smtk::common::UUIDs& selEntities);
  void getSelectedEntitiesAsEntityRefs(smtk::model::EntityRefs& selEntities);
  void getSelectedMeshes(smtk::mesh::MeshSets& selMeshes);

  // Description
  // a set store the name of all selection sources
  void getSelectionSources(std::set<std::string>& selectionSources);

  bool registerSelectionSource(std::string name)
  {
    return this->m_selectionSources.insert(name).second;
  }

  bool unregisterSelectionSource(std::string name)
  {
    return (this->m_selectionSources.erase(name) > 0);
  }

  void setSelectionModifierToAddition()
  {
    this->m_selectionModifier = SelectionModifier::SELECTION_ADDITION_FILTERED;
  }

  void setSelectionModifierToSubtraction()
  {
    this->m_selectionModifier = SelectionModifier::SELECTION_SUBTRACTION_FILTERED;
  }

signals:
  // Description
  // Broadcast selection (universally)
  // \a currently DesPhrases is always empty! we never pass DP into selectionManager.
  void broadcastToReceivers(const smtk::model::EntityRefs& selEntities,
    const smtk::mesh::MeshSets& selMeshes, const smtk::model::DescriptivePhrases& DesPhrases,
    const std::string& incomingSelectionSource);

public slots:
  // Description
  // this SLOT is used to update selected items
  // SelectionModifer is used to specify how to update selectionManager
  // selectionSource is used to specify which receiver you want to skip. Ex. you do
  // not want the current selection to update render window since the update
  // is coming from render window.
  // selectionSource: ${className}_${memoryAddress}
  // Ex class. qtModelView, qtAssociationWidget and pqSmtkModelPanel
  void updateSelectedItems(const smtk::model::EntityRefs& selEntities,
    const smtk::mesh::MeshSets& selMeshes, const smtk::model::DescriptivePhrases& DesPhrases,
    const smtk::extension::SelectionModifier modifierFlag, const std::string& selectionSource);

  // Description
  // update mask for models
  void filterModels(bool checked);

  // Description
  // update mask for volumes
  void filterVolumes(bool checked);

  // Description
  // update mask for faces
  void filterFaces(bool checked);

  // Description
  // update mask for edges
  void filterEdges(bool checked);

  // Description
  // update mask for vertices
  void filterVertices(bool checked);

  // Description
  // toggle filtering meshes
  void filterMeshes(bool checked);

  // Description
  // clear all selections by signal
  void clearAllSelections();

protected:
  void clear();
  // Description
  // filter select entities from inputSelEnt and store the result in
  // filteredSelEnt
  void filterEntitySelectionsByMaskAndActiveModel(
    smtk::model::EntityRefs& inputSelEnts, smtk::model::EntityRefs& filteredSelEnts);
  // Description
  // filter selection to handle mask other than F/E/V
  void filterRubberBandSelection(smtk::model::EntityRef ent);

  smtk::mesh::MeshSets m_selMeshes;
  smtk::model::BitFlags m_mask;
  smtk::model::DescriptivePhrases m_desPhrases;
  smtk::model::EntityRefs m_selEntityRefs;
  bool m_filterMeshes;
  smtk::model::ManagerPtr m_modelMgr;
  SelectionModifier m_selectionModifier;
  // a set store the name of the selection sources so that we do not need to
  // update the sender when broadcasting.
  // format: ${senderClassName}+${memoryAddress}
  std::set<std::string> m_selectionSources;
};

}; // namespace extension

}; // namespace smtk

#endif
