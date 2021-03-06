# set up sources to build
set(commonSrcs
  Color.cxx
  DateTime.cxx
  DateTimeZonePair.cxx
  Environment.cxx
  Extension.cxx
  FileLocation.cxx
  Paths.cxx
  Resource.cxx
  ResourceComponent.cxx
  ResourceSet.cxx
  StringUtil.cxx
  TimeZone.cxx
  UUID.cxx
  UUIDGenerator.cxx
  View.cxx
)

set(commonHeaders
  Color.h
  CompilerInformation.h
  DateTime.h
  DateTimeZonePair.h
  Environment.h
  Extension.h
  FileLocation.h
  GeometryUtilities.h
  Paths.h
  RangeDetector.h
  Resource.h
  ResourceComponent.h
  ResourceSet.h
  StringUtil.h
  TimeZone.h
  UUID.h
  UUIDGenerator.h
  View.h
  ${CMAKE_CURRENT_BINARY_DIR}/Version.h
)

if (APPLE)
  set(commonSrcs ${commonSrcs} PathsHelperMacOSX.mm)
  set(commonHeaders ${commonHeaders} PathsHelperMacOSX.h)
endif()
if (UNIX)
  set(commonSrcs ${commonSrcs} PathsHelperUnix.cxx)
  set(commonHeaders ${commonHeaders} PathsHelperUnix.h)
endif()
if (WIN32)
  set(commonSrcs ${commonSrcs} PathsHelperWindows.cxx)
  set(commonHeaders ${commonHeaders} PathsHelperWindows.h)
endif()

# Generate the version header using CMake variables.
configure_file(
  ${SMTK_SOURCE_DIR}/CMake/Version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/Version.h
  @ONLY
)

if (SMTK_ENABLE_PYTHON_WRAPPING)
  list(APPEND commonSrcs
    PythonInterpreter.cxx
    )
  list(APPEND commonHeaders
    PythonInterpreter.h
    )

  add_subdirectory(pybind11)
endif()

#install the headers
smtk_public_headers(${commonHeaders})

if (SMTK_ENABLE_TESTING)
  add_subdirectory(testing)
endif()
