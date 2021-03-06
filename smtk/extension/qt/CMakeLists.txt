find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# set up sources to build
set(QAttrLibSrcs
  qtActiveObjects.cxx
  qtUIManager.cxx
  qtSelectionManager.cxx
  qtAttribute.cxx
  qtAttributeDisplay.cxx
  qtAttributeItemWidgetFactory.cxx
  qtBaseView.cxx
  qtCheckItemComboBox.cxx
  qtCollapsibleGroupWidget.cxx
  qtEntityItemDelegate.cxx
  qtEntityItemEditor.cxx
  qtEntityItemModel.cxx
  qtGroupView.cxx
  qtAttributeView.cxx
  qtInstancedView.cxx
  qtOperatorView.cxx
  #qtModelView.cxx
  qtSelectorView.cxx
  qtSimpleExpressionView.cxx
  qtViewInfoDialog.cxx
  qtFileItem.cxx
  qtAttributeRefItem.cxx
  qtTableWidget.cxx
  qtAssociationWidget.cxx
  qtReferencesWidget.cxx
  qtColorButton.cxx

  qtItem.cxx
  qtDiscreteValueEditor.cxx
  qtInputsItem.cxx
  qtGroupItem.cxx
  qtVoidItem.cxx
  qtModelView.cxx
  qtModelPanel.cxx
  qtNewAttributeWidget.cxx
  qtOverlay.cxx
  qtMeshItem.cxx
  qtMeshSelectionItem.cxx
  qtModelEntityItem.cxx
  qtModelOperationWidget.cxx
  qtOperatorDockWidget.cxx
  qtTimeZoneRegionModel.cxx
  qtTimeZoneSelectWidget.cxx
  qtViewInterface.cxx
  qtSMTKUtilities.cxx
  qtDateTimeItem.cxx
  )

set(QAttrLibUIs
  qtAttributeAssociation.ui
  qtCollapsibleGroupWidgetInternals.ui
  qtModelPanel.ui
  qtNewAttributeWidget.ui
  qtTimeZoneSelectWidget.ui
  qtViewInfoDialog.ui
)


set(QAttrLibMocHeaders
  qtActiveObjects.h
  qtUIManager.h
  qtSelectionManager.h
  qtAttribute.h
  qtAttributeDisplay.h
  qtBaseView.h
  qtCheckItemComboBox.h
  qtCollapsibleGroupWidget.h
  qtEmittingStringBuffer.h
  qtEntityItemDelegate.h
  qtEntityItemEditor.h
  qtEntityItemModel.h
  qtGroupView.h
  qtAttributeView.h
  qtInstancedView.h
  qtOperatorView.h
  #qtModelView.h
  qtSelectorView.h
  qtSimpleExpressionView.h
  qtViewInfoDialog.h
  qtFileItem.h
  qtAttributeRefItem.h
  qtTableWidget.h
  qtAssociationWidget.h
  qtReferencesWidget.h
  qtColorButton.h

  qtItem.h
  qtDiscreteValueEditor.h
  qtInputsItem.h
  qtGroupItem.h
  qtVoidItem.h
  qtModelView.h
  qtModelPanel.h
  qtNewAttributeWidget.h
  qtOverlay.h
  qtMeshItem.h
  qtMeshSelectionItem.h
  qtModelEntityItem.h
  qtModelOperationWidget.h
  qtOperatorDockWidget.h
  qtDateTimeItem.h
  qtTimeZoneRegionModel.h
  qtTimeZoneSelectWidget.h
)

set(QAttrLibHeaders
  ${QAttrLibMocHeaders}
  qtAttributeItemWidgetFactory.h
  qtViewInterface.h
  qtSMTKUtilities.h
)

#install the headers
smtk_public_headers(${QAttrLibHeaders})

qt5_wrap_cpp(MOC_BUILT_SOURCES ${QAttrLibMocHeaders})
qt5_wrap_ui(SMTKQTEXT_UI_BUILT_SOURCES ${QAttrLibUIs})
qt5_add_resources(RCS_RESOURCES qtEntityItemModelIcons.qrc qtAttributeIcons.qrc)

source_group("Qt_Source" FILES ${QAttrLibSrcs} ${QAttrLibUIs})
source_group("Qt_Generated" FILES ${MOC_BUILT_SOURCES} ${SMTKQTEXT_UI_BUILT_SOURCES} ${RCS_RESOURCES})
source_group("Qt_Header" FILES ${QAttrLibMocHeaders})

add_library(smtkQtExt
  ${QAttrLibSrcs}
  ${MOC_BUILT_SOURCES}
  ${SMTKQTEXT_UI_BUILT_SOURCES}
  ${RCS_RESOURCES}
)

#we need to add the location of the moc files to the include dir for qtsmtk
target_include_directories(smtkQtExt PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#set smtkQtExt to publicly link to smtkCore
target_link_libraries(smtkQtExt LINK_PUBLIC
  smtkCore
  Qt5::Core
  Qt5::Widgets
  )
smtk_export_header(smtkQtExt Exports.h)

#install the library and exports the library when used from a build tree
smtk_install_library(smtkQtExt)

if (SMTK_ENABLE_TESTING)
  # ... and make header compilation tests link properly:
  target_link_libraries(TestBuild_smtk_extension_qt LINK_PRIVATE smtkQtExt)
endif()

if (SMTK_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

add_subdirectory(testing)
