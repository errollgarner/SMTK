PROJECT(SMTKDiscreteModel)

# The configured header file used to pass CMake settings to the source code.
configure_file(
  "${PROJECT_SOURCE_DIR}/Model/VTKUUIDConfigure.h.in"
  "${PROJECT_BINARY_DIR}/VTKUUIDConfigure.h"
  )


###############################################################################
###############################################################################
# UUID stuff. Probably don't need all of it
set(UUID_LIBS)
if(WIN32)
 if (NOT BORLAND)
    # rpcrt4 -> UuidCreate
    #snmpapi -> getting MAC address
    set(UUID_LIBS ${KIT_LIBS} rpcrt4 snmpapi)
  endif(NOT BORLAND)
endif(WIN32)

# Check if header file exists and add it to the list.
include (${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
MACRO(CHECK_INCLUDE_FILE_CONCAT FILE VARIABLE)
  CHECK_INCLUDE_FILES("${KWECommon_INCLUDES};${FILE}" ${VARIABLE})
  if(${VARIABLE})
    set(KWECommon_INCLUDES ${KWECommon_INCLUDES} ${FILE})
  endif(${VARIABLE})
ENDMACRO(CHECK_INCLUDE_FILE_CONCAT)

# Need these headers for GetMacAddress in vtkKWEUUID
# Tricky as you need to figure out the proper order to test the inclusion of files
CHECK_INCLUDE_FILE_CONCAT("unistd.h"       CMAKE_HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CONCAT("stdlib.h"       CMAKE_HAVE_STDLIB_H)
if(UNIX) #Avoid polluting Win32 cmakecache
  CHECK_INCLUDE_FILE_CONCAT("sys/ioctl.h"    CMAKE_HAVE_SYS_IOCTL_H)
  CHECK_INCLUDE_FILE_CONCAT("sys/socket.h"   CMAKE_HAVE_SYS_SOCKET_H)
  CHECK_INCLUDE_FILE_CONCAT("sys/sockio.h"   CMAKE_HAVE_SYS_SOCKIO_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if.h"       CMAKE_HAVE_NET_IF_H)
  CHECK_INCLUDE_FILE_CONCAT("netinet/in.h"   CMAKE_HAVE_NETINET_IN_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if_dl.h"    CMAKE_HAVE_NET_IF_DL_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if_arp.h"   CMAKE_HAVE_NET_IF_ARP_H)
endif(UNIX)

# Test if os defines a length for sockaddr
# only doing the test when we have sys/socket
if( ${CMAKE_HAVE_SYS_SOCKET_H} )
  if(NOT DEFINED "HAVE_SA_LEN")
    STRING(ASCII 35 POUND)
    FILE(WRITE ${PROJECT_BINARY_DIR}/CMakeTmp/kweCommonTestHAVESALEN.c
      "${POUND}include <sys/types.h>\n"
      "${POUND}include <sys/socket.h>\n"
      "int main() { struct sockaddr sa; sa.sa_len = 0; }\n")
    message(STATUS "Checking if ifreq has a sa_len")
    TRY_COMPILE(HAVE_SA_LEN
      ${PROJECT_BINARY_DIR}
      ${PROJECT_BINARY_DIR}/CMakeTmp/kweCommonTestHAVESALEN.c
      OUTPUT_VARIABLE OUTPUT)
    if(HAVE_SA_LEN)
      message(STATUS "Checking if ifreq has a sa_len -- yes")
      set(HAVE_SA_LEN 1 CACHE INTERNAL "Support if ifreq has a sa_len")
      write_file(${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Determining if ifreq has a sa_len"
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    else(HAVE_SA_LEN)
      message(STATUS "Checking if ifreq has a sa_len -- no")
      set(HAVE_SA_LEN 0 CACHE INTERNAL "Support if ifreq has a sa_len")
      write_file(${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Determining if ifreq has a sa_len"
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    endif(HAVE_SA_LEN)
  endif()
endif( ${CMAKE_HAVE_SYS_SOCKET_H} )
###############################################################################
###############################################################################

SET(ModelFiles
  Serialize/vtkSerializableObject.cxx
  Serialize/vtkSerializationHelper.cxx
  Serialize/vtkSerializationHelperMap.cxx
  Serialize/vtkSerializer.cxx
  Serialize/vtkModelXMLParser.cxx
  Serialize/vtkCommonSerializationHelper.cxx
  Serialize/vtkInformationKeyMap.cxx
  Serialize/vtkRenderingSerializationHelper.cxx
  Serialize/vtkXMLArchiveReader.cxx
  Serialize/vtkXMLArchiveWriter.cxx
  Serialize/vtkXMLElement.cxx
  Model/vtkModelItem.cxx
  Model/vtkModelItemIterator.cxx
  Model/vtkModelItemGenericIterator.cxx
  Model/vtkModelItemListIterator.cxx
  Model/vtkModelEntity.cxx
  Model/vtkModel.cxx
  Model/vtkModelGeometricEntity.cxx
  Model/vtkModelVertex.cxx
  Model/vtkModelVertexUse.cxx
  Model/vtkModelEdge.cxx
  Model/vtkModelEdgeUse.cxx
  Model/vtkModelLoopUse.cxx
  Model/vtkModelFace.cxx
  Model/vtkModelFaceUse.cxx
  Model/vtkModelShellUse.cxx
  Model/vtkModelRegion.cxx
  Model/vtkUUID.cxx
)

SET(DiscreteModelFiles
  DiscreteMesh.cxx
  DiscreteMeshCellIdIterator.h
  MeshClassification.h
  ModelEdgeHelper.h
  ModelParserHelper.h
  ModelVertexClassification.cxx
  vtkModel3dmGridRepresentation.cxx
  vtkModel3dm2DGridRepresentation.cxx
  vtkModelBCGridRepresentation.cxx
  vtkModelGridRepresentation.cxx
  vtkModelGeneratedGridRepresentation.cxx
  vtkDiscreteModel.cxx
  vtkModelMaterial.cxx
  vtkDiscreteModelEdge.cxx
  vtkDiscreteModelEntity.cxx
  vtkDiscreteModelEntityGroup.cxx
  vtkDiscreteModelFace.cxx
  vtkDiscreteModelGeometricEntity.cxx
  vtkDiscreteModelRegion.cxx
  vtkDiscreteModelVertex.cxx
  #vtkDiscreteModelWrapper.cxx
  vtkModelUserName.cxx
  vtkXMLModelReader.cxx
  vtkXMLModelWriter.cxx
  vtkMergeEventData.cxx
  vtkSplitEventData.cxx
)

SET_SOURCE_FILES_PROPERTIES(
  vtkDiscreteModel
  vtkDiscreteModelGeometricEntity
  vtkModelGridRepresentation
  vtkModelGeneratedGridRepresentation
  Model/vtkModel
  Model/vtkModelEdge
  Model/vtkModel
  Model/vtkModelFace
  Model/vtkModelGeometricEntity
  Model/vtkModelItem
  Model/vtkModelItemIterator
  Model/vtkModelRegion
  Model/vtkModelVertex
  ABSTRACT
)

SET(DiscreteModelHeaders
  Serialize/vtkSerializableObject.h
  Serialize/vtkSerializationHelper.h
  Serialize/vtkSerializationHelperMap.h
  Serialize/vtkSerializer.h
  Serialize/vtkModelXMLParser.h
  Serialize/vtkCommonSerializationHelper.h
  Serialize/vtkInformationKeyMap.h
  Serialize/vtkRenderingSerializationHelper.h
  Serialize/vtkXMLArchiveReader.h
  Serialize/vtkXMLArchiveWriter.h
  Serialize/vtkXMLElement.h
  Model/vtkModelItem.h
  Model/vtkModelItemIterator.h
  Model/vtkModelItemGenericIterator.h
  Model/vtkModelItemListIterator.h
  Model/vtkModel.h
  Model/vtkModelEntity.h
  Model/vtkModelGeometricEntity.h
  Model/vtkModelVertex.h
  Model/vtkModelVertexUse.h
  Model/vtkModelEdge.h
  Model/vtkModelEdgeUse.h
  Model/vtkModelLoopUse.h
  Model/vtkModelFace.h
  Model/vtkModelFaceUse.h
  Model/vtkModelShellUse.h
  Model/vtkModelRegion.h
  Model/vtkUUID.h

  DiscreteMesh.h
  DiscreteMeshCellIdIterator.h
  MeshClassification.h
  ModelEdgeHelper.h
  ModelVertexClassification.h
  vtkModel3dmGridRepresentation.h
  vtkModel3dm2DGridRepresentation.h
  vtkModelBCGridRepresentation.h
  vtkModelGridRepresentation.h
  vtkModelGeneratedGridRepresentation.h
  vtkDiscreteModel.h
  vtkModelMaterial.h
  vtkDiscreteModelEdge.h
  vtkDiscreteModel.h
  vtkDiscreteModelEntity.h
  vtkDiscreteModelEntityGroup.h
  vtkDiscreteModelFace.h
  vtkDiscreteModelGeometricEntity.h
  vtkDiscreteModelRegion.h
  vtkDiscreteModelVertex.h
  vtkDiscreteModelWrapper.h
  vtkModelUserName.h
  vtkXMLModelReader.h
  vtkXMLModelWriter.h
  vtkMergeEventData.h
  vtkSplitEventData.h

  "${CMAKE_CURRENT_BINARY_DIR}/vtkSMTKDiscreteModelModule.h"
)

# Disable manifests for plugins on Windows so that they do not need to resolve their dependencies.
if (MSVC)
   set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif(MSVC)

SET_SOURCE_FILES_PROPERTIES(
  ${ModelFiles}
  ${DiscreteModelFiles}
  PROPERTIES
    WRAP_EXCLUDE
    WRAP_EXCLUDE_PYTHON
)

set(_module_src
  ${ModelFiles}
  ${DiscreteModelFiles}
  vtkDiscreteModelWrapper.cxx
)

set(vtkSMTKDiscreteModel_NO_HeaderTest 1)
set(VTK_INSTALL_NO_HEADERS 1)
vtk_module_library(vtkSMTKDiscreteModel ${_module_src})
target_include_directories(${vtk-module}
  PUBLIC
    $<BUILD_INTERFACE:${SMTK_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SMTK_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/smtk/${SMTK_VERSION}>)

target_link_libraries(vtkSMTKDiscreteModel LINK_PRIVATE ${UUID_LIBS})

# install the headers
smtk_public_headers(${DiscreteModelHeaders})

# ... and make header compilation tests link properly:
smtk_install_library(vtkSMTKDiscreteModel)

if (SMTK_ENABLE_PARAVIEW_SUPPORT)
  # This module currently has source files located in its sub directories,
  # so it will not be CS wrapped properly, and generate a lot of warning at CMake configure time.
  # Therefore we disable CS wrapping until the issue with source files in sub directories is addressed.
  # vtk_add_cs_wrapping(${vtk-module})
  # smtk_install_library(${vtk-module}CS)
endif ()

if (SMTK_ENABLE_TESTING)
  # ... and make header compilation tests link properly:
  target_link_libraries(TestBuild_smtk_bridge_discrete_kernel LINK_PRIVATE vtkCommonCore)
endif()
