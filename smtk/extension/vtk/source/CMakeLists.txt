set(VTKLibSrcs
  vtkAuxiliaryGeometryExtension.cxx
  vtkHydroModelPolySource.cxx
  vtkMeshMultiBlockSource.cxx
  vtkModelAuxiliaryGeometry.cxx
  vtkModelMultiBlockSource.cxx
  vtkModelRepresentation.cxx
  vtkModelSource.cxx
  vtkModelView.cxx
)

set(VTKLibHeaders
  vtkAuxiliaryGeometryExtension.h
  vtkHydroModelPolySource.h
  vtkMeshMultiBlockSource.h
  vtkModelAuxiliaryGeometry.h
  vtkModelMultiBlockSource.h
  vtkModelRepresentation.h
  vtkModelSource.h
  vtkModelView.h
  vtkTracksAllInstances.h
)

set_source_files_properties(
  vtkAuxiliaryGeometryExtension.h
  PROPERTIES
    WRAP_EXCLUDE 1
    WRAP_EXCLUDE_PYTHON 1
)

# Add compile definitions to force linking to implementation modules
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})

# Don't do VTK header test. It is broken at the moment.
set(${vtk-module}_NO_HeaderTest 1)
set(VTK_INSTALL_NO_HEADERS 1)
vtk_module_library(${vtk-module} ${VTKLibSrcs})
target_include_directories(${vtk-module}
  PUBLIC
    $<BUILD_INTERFACE:${SMTK_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SMTK_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/smtk/${SMTK_VERSION}>)

target_link_libraries(${vtk-module}
  LINK_PUBLIC
    smtkCore
    smtkIOVTK
  LINK_PRIVATE
    ${Boost_LIBRARIES}
)

#has to be after the vtk_module_library
smtk_export_header(${vtk-module} Exports.h)

target_include_directories(${vtk-module}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link *everything* in this directory to VTK modules
# This is required for the TestBuild targets.
# We must test whether each of the targets below exist
# because they are required when built into VTK but
# may not be present, especially since the OpenGL2
# backend became available.
set(SMTK_VTK_TARGET_LIST)
foreach(target
  vtkFiltersExtraction
  vtkFiltersGeometry
  vtkInteractionStyle
  vtkRenderingContext2D
  vtkRenderingFreeType
  vtkRenderingVolume
  vtkRenderingContextOpenGL2
  vtkRenderingOpenGL2
  vtkRenderingVolumeOpenGL2
  vtkRenderingGL2PSOpenGL2
  vtkIOGeometry
  vtkIOPLY
  vtkIOMPIParallel
  vtkIOMPIImage
  )
  if (TARGET ${target})
    list(APPEND SMTK_VTK_TARGET_LIST ${target})
  endif()
endforeach()

target_link_libraries(${vtk-module} PUBLIC ${SMTK_VTK_TARGET_LIST})

#install the headers
smtk_public_headers(${VTKLibHeaders})
# ... and make header compilation tests link properly:
smtk_install_library(${vtk-module})

if (SMTK_ENABLE_PARAVIEW_SUPPORT)
  vtk_add_cs_wrapping(${vtk-module})
  smtk_install_library(${vtk-module}CS)
endif()

if (SMTK_ENABLE_PYTHON_WRAPPING)
  if (SMTK_ENABLE_PARAVIEW_SUPPORT)
    vtk_add_python_wrapping(${vtk-module} VTKLibSrcs)
    vtk_add_python_wrapping_library(${vtk-module} VTKLibSrcs ${vtk-module})
  elseif (VTK_WRAP_PYTHON)
    # Potentially wrap in Python using VTK's wrapping methods.
    # This makes the filters available for use with other VTK-wrapped filters.
    vtk_module_load(${vtk-module})
    vtk_module_headers_load(${vtk-module})
    vtk_add_python_wrapping(${vtk-module} VTKLibSrcs)
    vtk_add_python_wrapping_library(${vtk-module} VTKLibSrcs ${vtk-module})
    smtk_install_library(${vtk-module}PythonD)
    smtk_install_library(${vtk-module}Python)
  endif()
endif()

if (SMTK_ENABLE_PYTHON_WRAPPING)
  add_subdirectory(pybind11)
endif()

if (SMTK_ENABLE_TESTING)
  target_link_libraries(TestBuild_smtk_extension_vtk_source PUBLIC ${SMTK_VTK_TARGET_LIST})
  add_subdirectory(testing)
endif()
