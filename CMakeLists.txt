cmake_minimum_required(VERSION 3.8.2)

cmake_policy(SET CMP0012 NEW)
cmake_policy(SET CMP0020 NEW)
if (POLICY CMP0028)
  cmake_policy(SET CMP0028 NEW)
endif()
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()
if (POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()

#If the user/superbuild hasn't explicitly stated what c++ standard to use
#require C++11
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(CMAKE_CXX_EXTENSIONS FALSE)
endif()

project(SMTK)

# Remove SMTKTargets.cmake at the beginning of each CMake run; each time
# a library is declared below, the file is appended and we do not want
# to keep appending over multiple runs.
file(REMOVE ${PROJECT_BINARY_DIR}/SMTKTargets.cmake)

#Add our Cmake directory to the module search path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SMTK_SOURCE_DIR}/CMake)

################################################################################
# SMTK version number
include(SMTKVersion)

################################################################################

# Include GNU install directory module to detect where to install
# files on Linux/Unix systems (e.g., lib vs lib64)
include(GNUInstallDirs)

# Include export header so that we can easily expose symbols
# in dynamic libraries
include(GenerateExportHeader)

# Include cmake code that builds automatic header tests,
# makes targets for unit tests, and makes it easier to generate
# export install targets
include(SMTKMacros)

# include cmake code that allows for easier testing of SMTK classes
# and also provides helpers around create_test_sourcelist
include(CMake/SMTKTestingMacros.cmake)

# Add options for performing code coverage tests.
include(CMake/SMTKCoverage.cmake)

# Include mechanism for determining function and bind support
include(Function)

# Include mechanism for determining how to specialize hash<X>()
include(FindHashFunctor)

################################################################################
## Paths for output

# Set the directory where the binaries will be stored
set(EXECUTABLE_OUTPUT_PATH
  ${PROJECT_BINARY_DIR}/bin)

# Set the directory where the libraries will be stored
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH
  ${PROJECT_BINARY_DIR}/lib)

include(CMakeDependentOption)

################################################################################
# Options that the user controls
################################################################################
option(BUILD_SHARED_LIBS "Build SMTK using shared libraries" ON)
option(SMTK_NO_SYSTEM_BOOST "Allow boost to search for system installed boost" ON)
# Option to build Qt ui compoments for attributes
option(SMTK_ENABLE_TESTING "Enable Testing" ON)
option(SMTK_ENABLE_EXAMPLES "Enable Examples" OFF)
option(SMTK_ENABLE_PYTHON_WRAPPING "Build Python Wrappings" OFF)
# Provide system packagers with the ability to install SMTK
# to the system's Python site package directory. The default
# is off so that people building relocatable bundles (such as
# CMB) can distribute their own Python interpreter with
# internal packages (e.g., as part of a Mac ".app" bundle).
cmake_dependent_option(
  SMTK_INSTALL_PYTHON_TO_SITE_PACKAGES
  "Install Python modules to the interpreter's site-packages directory or into CMAKE_INSTALL_PREFIX?"
  OFF
  SMTK_ENABLE_PYTHON_WRAPPING OFF)
mark_as_advanced(SMTK_INSTALL_PYTHON_TO_SITE_PACKAGES)
option(SMTK_ENABLE_QT_SUPPORT "Build Qt GUI" OFF)
cmake_dependent_option(SMTK_ENABLE_APPLICATIONS "Build Qt-enabled applications" ON SMTK_ENABLE_QT_SUPPORT OFF)
option(SMTK_ENABLE_VTK_SUPPORT "Build VTK component" ON)
# SMTK uses vxl library for terrain extraction. It also depends on VTK and ParaView.
cmake_dependent_option(SMTK_ENABLE_VXL_SUPPORT "Build VXL component" ON
  "SMTK_ENABLE_PARAVIEW_SUPPORT;SMTK_ENABLE_QT_SUPPORT;SMTK_ENABLE_VTK_SUPPORT" OFF)
option(SMTK_ENABLE_REMUS_SUPPORT "Build Remus components" OFF)
cmake_dependent_option(SMTK_ENABLE_PARAVIEW_SUPPORT "Build paraview plugins for model sessions" OFF
  SMTK_ENABLE_VTK_SUPPORT OFF)

option(SMTK_ENABLE_CGM_SESSION "Build CGM component" OFF)
cmake_dependent_option(SMTK_ENABLE_DISCRETE_SESSION "Build discrete model session components" OFF
  SMTK_ENABLE_VTK_SUPPORT OFF)
cmake_dependent_option(SMTK_ENABLE_MOAB_DISCRETE_READER "Build moab reader for discrete model" OFF
  "SMTK_ENABLE_DISCRETE_SESSION" OFF)
cmake_dependent_option(SMTK_ENABLE_POLYGON_SESSION "Build Boost.polygon model session." ON
  SMTK_ENABLE_VTK_SUPPORT OFF)
cmake_dependent_option(SMTK_ENABLE_EXODUS_SESSION "Build a session to Exodus-II side sets" ON
  SMTK_ENABLE_VTK_SUPPORT OFF)
cmake_dependent_option(SMTK_ENABLE_REMOTE_SESSION "Build a session that allows modeling in a remote process" ON
  SMTK_ENABLE_REMUS_SUPPORT OFF)
option(SMTK_ENABLE_MESH_SESSION
  "Build a session that uses smtk::mesh as its backing" ON)
cmake_dependent_option(SMTK_ENABLE_MULTISCALE_SESSION "Build a session to Multiscale data" OFF
  "SMTK_ENABLE_VTK_SUPPORT;SMTK_ENABLE_MESH_SESSION" OFF)
if (SMTK_ENABLE_MULTISCALE_SESSION)
  set(AFRL_DIR "" CACHE PATH "Path to a directory of AFRL data.")
endif()

option(SMTK_USE_SYSTEM_MOAB "Use the system-installed moab?" OFF)
option(SMTK_USE_SYSTEM_SPARSEHASH "Use the system-installed sparsehash?" OFF)
option(SMTK_HASH_STORAGE "Use sparsehash library for primary storage?" OFF)
mark_as_advanced(SMTK_USE_SYSTEM_SPARSEHASH SMTK_HASH_STORAGE)

option(SMTK_ENABLE_DOCUMENTATION
  "Include targets for Doxygen- and Sphinx-generated documentation" OFF)
if (SMTK_ENABLE_DOCUMENTATION)
  find_package(Doxygen)
  find_package(Sphinx)
endif()

cmake_dependent_option(
  SMTK_ENABLE_CUMULUS_SUPPORT
  "Build cumulus interface and GUI (requires Qt w/OpenSSL support"
  OFF
  SMTK_ENABLE_QT_SUPPORT
  OFF)

################################################################################
# Testing Related Settings
################################################################################

#turn on ctest if we want testing
if (SMTK_ENABLE_TESTING)
  enable_testing()
  include(CTest)

  unset(SMTK_DATA_DIR CACHE)
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/data/smtk-data" data)
  if (NOT data STREQUAL "\n")
    message(WARNING
      "Testing is enabled, but the data is not available. Use git-lfs in order "
      "to obtain the testing data.")
    set(SMTK_DATA_DIR)
  else ()
    set(SMTK_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
  endif ()

  set(SMTK_COVERAGE_ENABLED OFF)
  if(SMTK_ENABLE_COVERAGE)
    set(SMTK_COVERAGE_ENABLED ON)
  endif()

  # Do not report some warnings from generated code to the dashboard:
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/CTestCustom.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake")

  # Add a test to check for the copyright statment in all source:
  add_test(NAME CopyrightStatement
    COMMAND ${CMAKE_COMMAND}
        "-DSMTK_SOURCE_DIR=${SMTK_SOURCE_DIR}"
        -P "${SMTK_SOURCE_DIR}/CMake/CheckCopyright.cmake")
endif()

################################################################################
# Boost Related Settings
################################################################################

# Boost is required for boost::uuids::uuid and, depending
# on the platform, for filesystem support.
#setup if we should use boost static libraries based on if we are
#building static or shared. We need to match boosts library type to ours so
#that we handle symbol visibility properly, plus at the same time we have
#to understand that on some platforms or builds we will only have the static
#libraries so we need to handle that when presented
if(NOT DEFINED Boost_USE_STATIC_LIBS)
  if(${BUILD_SHARED_LIBS})
    set(Boost_USE_STATIC_LIBS OFF)
  else()
    set(Boost_USE_STATIC_LIBS ON)
  endif()
endif()

#if boost is installed as a system library on a machine, we will never
#be able to set the superbuild to use the custom boost version. The solution
#is that when doing a system
set(Boost_NO_SYSTEM_PATHS ${SMTK_NO_SYSTEM_BOOST})

# List of Boost features used:
# * Date Time
# * Filesystem
# * String algorithms
# * UUID Generation
set(required_boost_components
  date_time filesystem system)

# Some c++ compilers do not support regex, so we may need Boost's regex library.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND
      (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0")) OR
    (WIN32 AND MSVC))
#we definitely do not need regex support
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND
    CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0")
#we definitely need regex support
  list(APPEND required_boost_components regex)
else()
#we may need regex support, but rather than try-compile let's just use boost
  list(APPEND required_boost_components regex)
endif()

find_package(Boost 1.64.0
             COMPONENTS ${required_boost_components} REQUIRED)

if(WIN32 AND MSVC)
  #setup windows exception handling so we can compile properly with boost
  #enabled
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  #quiet warnings about printf being potentially unsafe
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
  #quiet warnings about truncating decorated name
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4503")
  add_definitions(-DBOOST_ALL_NO_LIB)
endif()

# Setting this ensures that "make install" will leave rpaths to external
# libraries (not part of the build-tree e.g. Qt, ffmpeg, etc.) intact on
# "make install". This ensures that one can install a version of ParaView on the
# build machine without any issues. If this not desired, simply specify
# CMAKE_INSTALL_RPATH_USE_LINK_PATH when configuring and "make install" will
# strip all rpaths, which is default behavior.
if (NOT CMAKE_INSTALL_RPATH_USE_LINK_PATH)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()

################################################################################
# Moab Related Settings
################################################################################

#if we are not using system moab we will build our own version through
#the thirdparty folder
if (SMTK_USE_SYSTEM_MOAB)
  # Moab is required for building smtk::mesh, as it is the default storage
  # format for meshes.
  find_package(MOAB REQUIRED)
endif()


################################################################################
# Qt Related Settings
################################################################################

################################################################################
# NOTE: This has be before any find_package(VTK), because currently ParaView
# and VTK can NOT be easily switched due to modules and macros intermixed
################################################################################
################################################################################
# Look for Discrete session discrete model modules
################################################################################

if(SMTK_ENABLE_PARAVIEW_SUPPORT)
  # include ParaView and ensure it is using the OpenGL2 backend
  find_package(ParaView)
endif()

################################################################################
# VTK Related Settings
################################################################################

if(SMTK_ENABLE_VTK_SUPPORT AND NOT SMTK_ENABLE_PARAVIEW_SUPPORT)
  # Find the package here so environment variables are set, but
  # be careful not to include the VTK_USE_FILE until inside
  # a subdirectory that requires it; otherwise the list of include
  # paths can become too long for some platforms.
  find_package(VTK REQUIRED)
endif()

if (SMTK_ENABLE_PARAVIEW_SUPPORT OR SMTK_ENABLE_VTK_SUPPORT)
  if(NOT VTK_RENDERING_BACKEND STREQUAL "OpenGL2")
    message(FATAL_ERROR
      "SMTK requires that VTK be built with the OpenGL2 Rendering backend.")
  endif()
endif()

################################################################################
# CGM Related Settings
################################################################################

if(SMTK_ENABLE_CGM_SESSION)
  find_package(CGM REQUIRED)
endif(SMTK_ENABLE_CGM_SESSION)

################################################################################
# Remus related settings
################################################################################

if (SMTK_ENABLE_REMUS_SUPPORT)
  find_package(Remus REQUIRED)
endif()

################################################################################
# Sparsehash Related Settings
################################################################################

if (SMTK_USE_SYSTEM_SPARSEHASH)
  # A system sparsehash implementation must be detected with pkgconfig:
  set(SYSTEM_SPARSEHASH_REQUIRED "REQUIRED")
  include(FindPkgConfig)
  pkg_check_modules(SPARSEHASH ${SYSTEM_SPARSEHASH_REQUIRED} libsparsehash)
else()
  set(SPARSEHASH_INCLUDE_DIRS
    "${SMTK_SOURCE_DIR}/thirdparty"
    "${SMTK_BINARY_DIR}/thirdparty")
endif()

################################################################################
# Build third party libraries
################################################################################

add_subdirectory(thirdparty)

################################################################################
# Wrapping Related Settings
################################################################################
if(SMTK_ENABLE_PYTHON_WRAPPING)
  find_package(PythonLibs REQUIRED)
  find_package(PythonInterp 2.7 REQUIRED)

  # Initialize SMTK_PYTHON_MODULEDIR.
  # This stores the location where we'll install SMTK's Python modules.
  # Note that SMTK_PYTHON_MODULEDIR may be provided if SMTK is being
  # built as a submodule or as part of a superbuild.
  if (NOT DEFINED SMTK_PYTHON_MODULEDIR)
    if (SMTK_INSTALL_PYTHON_TO_SITE_PACKAGES)
        execute_process(
          COMMAND
          ${PYTHON_EXECUTABLE}
          -c "import site; print(site.getsitepackages())[-1]"
          RESULT_VARIABLE SMTK_PYTHON_MODULEDIR
          )
    elseif(WIN32)
        set(SMTK_PYTHON_MODULEDIR
          "bin/Lib/site-packages")
    else()
        set(SMTK_PYTHON_MODULEDIR
          "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages")
    endif()
  endif()


  if(SMTK_ENABLE_PYTHON_WRAPPING)
    if(WIN32)
      set(PYTHON_MODULE_EXTENSION ".pyd")
    else()
      set(PYTHON_MODULE_EXTENSION ".so")
    endif()
    set(PYTHON_MODULE_PREFIX "")
    find_package(pybind11 REQUIRED)

    set(SMTK_PYBIND11_FLAGS " ")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
       CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
       CMAKE_CXX_COMPILER_ID MATCHES "Intel")
      set(SMTK_PYBIND11_FLAGS "${SMTK_PYBIND11_FLAGS} -Wno-shadow")
    endif()
  endif()


  # Set up environment variables needed to import the python modules for tests.
  if (PYTHONINTERP_FOUND AND SMTK_ENABLE_TESTING)
    set(required_python_modules)

    if (SMTK_ENABLE_PARAVIEW_SUPPORT)
      list(APPEND required_python_modules
        paraview)
      set(paraview_PYTHONPATH
        "${PARAVIEW_PYTHONPATH}")
      set(paraview_libpath
        "${PARAVIEW_RUNTIME_DIRS}")
    elseif (SMTK_ENABLE_VTK_SUPPORT)
      list(APPEND required_python_modules
        vtk)
      # TODO: VTK needs to export a variable for this
      set(vtk_PYTHONPATH
        "${VTK_INSTALL_PREFIX}/lib/python2.7/site-packages"
        "${VTK_INSTALL_PREFIX}/lib")
      set(vtk_libpath
        "${VTK_INSTALL_PREFIX}/lib")
    endif ()

    set(envsep ":")
    if (WIN32)
      set(envsep ";")
    endif ()

    set(libpath_envvar)
    if (WIN32)
      set(libpath_envvar "PATH")
    elseif (APPLE)
      set(libpath_envvar "DYLD_FALLBACK_LIBRARY_PATH")
    elseif (UNIX)
      set(libpath_envvar "LD_LIBRARY_PATH")
    else ()
      message(WARNING "Unknown platform loader path environment variable.")
    endif ()

    function (smtk_test_python_module output_python output_lib module pythonpath libpath)
      string(REPLACE ";" "${envsep}" pythonpath_env "${pythonpath}")

      if (DEFINED ENV{PYTHONPATH})
        set(pythonpath_env
          "${pythonpath_env}${envsep}$ENV{PYTHONPATH}")
      endif ()

      string(REPLACE ";" "${envsep}" libpath_env "${libpath}")

      if (DEFINED ENV{${libpath_envvar}})
        set(libpath_env
          "${libpath_env}${envsep}$ENV{${libpath_envvar}}")
      endif ()

      # Test the PYTHONPATH passed in.
      execute_process(
        COMMAND "${CMAKE_COMMAND}"
                -E env
                "PYTHONPATH=${pythonpath_env}"
                "${libpath_envvar}=${libpath_env}"
                "${PYTHON_EXECUTABLE}"
                -c "import ${module}"
        RESULT_VARIABLE res
        OUTPUT_VARIABLE out
        ERROR_VARIABLE  err)

      if (res)
        message(SEND_ERROR
          "Failed to import the ${module} Python module. The attempted "
          "PYTHONPATH was '${pythonpath_env}' and ${libpath_envvar} set to "
          "'${libpath_env}', but it still failed. Please set the PYTHONPATH "
          "environment to make the ${module} module successfully import. The "
          "error message was '${err}'.")

        return ()
      endif ()

      # Add to the list of paths required for modules.
      set("${output_python}"
        ${${output_python}}
        "${pythonpath}"
        PARENT_SCOPE)

      # Add to the list of paths required for modules.
      set("${output_lib}"
        ${${output_lib}}
        "${libpath}"
        PARENT_SCOPE)
    endfunction ()

    set(smtk_pythonpaths
      "$ENV{PYTHONPATH}")
    set(smtk_libpaths
      "$ENV{${libpath_envvar}}")
    foreach (module IN LISTS required_python_modules)
      smtk_test_python_module(smtk_pythonpaths smtk_libpaths
        "${module}" "${${module}_PYTHONPATH}" "${${module}_libpath}")
    endforeach ()

    # Add the path to the build tree's compiled modules, prioritizing it over
    # the other directories (since the other directories will likely include the
    # location of the module's install).
    list(INSERT smtk_pythonpaths 0
      "${PROJECT_BINARY_DIR}${envsep}${PROJECT_BINARY_DIR}/lib")
    list(INSERT smtk_libpaths 0
      "${PROJECT_BINARY_DIR}/lib")
    if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
      list(INSERT smtk_libpaths 0
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endif ()

    # Remove duplicates.
    list(REMOVE_DUPLICATES smtk_pythonpaths)
    list(REMOVE_DUPLICATES smtk_libpaths)

    string(REPLACE ";" "${envsep}" smtk_pythonpath_env "${smtk_pythonpaths}")
    string(REPLACE ";" "${envsep}" smtk_libpath_env "${smtk_libpaths}")
    if (WIN32)
      string(REPLACE "/" "\\" smtk_libpath_env "${smtk_libpath_env}")
    endif ()
    string(REPLACE ";" "\;" smtk_pythonpath_env "${smtk_pythonpath_env}")
    string(REPLACE ";" "\;" smtk_libpath_env "${smtk_libpath_env}")
    set(smtk_python_test_environment
      "PYTHONPATH=${smtk_pythonpath_env}${envsep}${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
      "${libpath_envvar}=${smtk_libpath_env}")

    function (smtk_add_test_python name file)
      add_test(
        NAME    "${name}"
        COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/${file}"
                ${ARGN})
      set_tests_properties("${name}"
        PROPERTIES
          ENVIRONMENT "${smtk_python_test_environment}")
    endfunction ()
  endif()
endif()

################################################################################
# Determine function
################################################################################
determineFunctionType(
  FUNCTION_FOUND
  FUNCTION_NAMESPACE
  USING_PLACEHOLDERS
  FUNCTION_TYPE_HEADERS)

if (NOT FUNCTION_FOUND)
  message(FATAL_ERROR
    "
    SMTK requires std::function, std::tr1::function, or boost::function.
    Please examine CMake's logs to see why CMake/function*.cxx won't compile.
    ")
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/CMake/Function.h.in
  ${PROJECT_BINARY_DIR}/smtk/Function.h
  @ONLY)

install (FILES ${PROJECT_BINARY_DIR}/smtk/Function.h
  DESTINATION include/smtk/${SMTK_VERSION}/smtk)

################################################################################
# Determine hash<X> specialization
################################################################################
find_hash_functor(SMTK_HASH_H SMTK_HASH_NS SMTK_HASH_BEGIN_NS SMTK_HASH_END_NS SMTK_HASH_SPECIALIZATION)
if (NOT SMTK_HASH_H)
  message(FATAL_ERROR "Failed to find a header which provides std::hash<> (or equivalent).")
endif ()
configure_file(
  ${PROJECT_SOURCE_DIR}/CMake/HashFunctor.h.in
  ${PROJECT_BINARY_DIR}/smtk/HashFunctor.h
  @ONLY)

install (FILES ${PROJECT_BINARY_DIR}/smtk/HashFunctor.h
  DESTINATION include/smtk/${SMTK_VERSION}/smtk)

find_file(CGM_HAVE_VERSION_H
  NAMES cgm_version.h
  PATHS ${CGM_INCLUDE_DIRS}
  DOC   "Location of CGM's cgm_version.h")

################################################################################
# Save compile-time options for use by other packages
################################################################################
configure_file(
  ${PROJECT_SOURCE_DIR}/CMake/Options.h.in
  ${PROJECT_BINARY_DIR}/smtk/Options.h
  @ONLY)

install (FILES ${PROJECT_BINARY_DIR}/smtk/Options.h
  DESTINATION include/smtk/${SMTK_VERSION}/smtk)


################################################################################
# Install Related Settings
################################################################################

# Install rules for SMTK macros usable by external packages:
install(
  FILES
    ${PROJECT_SOURCE_DIR}/CMake/EncodeCStringFunctions.cmake
    ${PROJECT_SOURCE_DIR}/CMake/EncodePyStringFunctions.cmake
    ${PROJECT_SOURCE_DIR}/CMake/SMTKOperatorXML.cmake
    ${PROJECT_SOURCE_DIR}/CMake/SMTKSessionJSON.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/SMTK
)

################################################################################
# Include Dirs Settings
################################################################################

# Add the current directory so we can include files like: <smtk/internal/Test.h>
# Save the include directories in a variable so that VTK modules
# (inside the extension/vtk and bridge/discrete directories) can reuse them.
include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
include_directories(
  SYSTEM
  "${SPARSEHASH_INCLUDE_DIRS}"
  "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cJSON"
  "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/pugixml"
  "${Boost_INCLUDE_DIRS}"
)

################################################################################
# Finally build smtk
################################################################################

# On Mac OS X, set the dir. included as part of the installed library's path:
if (BUILD_SHARED_LIBS AND NOT DEFINED CMAKE_INSTALL_NAME_DIR)
  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif ()

add_subdirectory(smtk)

# Build SMTK Applications
if(SMTK_ENABLE_APPLICATIONS)
  add_subdirectory(applications)
endif()

################################################################################
# Install a package targets file
#
# Setup the exports for the library when used from an installed location
install(
  EXPORT SMTK
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SMTK
  FILE SMTKTargets.cmake
)

export(EXPORT SMTK FILE "${PROJECT_BINARY_DIR}/SMTKTargets.cmake")

################################################################################
# Create a build directory package configuration file
#
# Setup the config file for exports that stores what other thirdparty
# packages we need to search for ( MOAB, Remus, etc ) for when using the
# build version of smtk
#
# Required to happen before we include the docs directory
set(MOAB_ROOT_DIR "${MOAB_ROOT_DIR}")
set(SMTK_VTK_MODULE_DIR "${CMAKE_BINARY_DIR}/lib/cmake/SMTK")
set(SMTK_MODULE_DIR "${PROJECT_SOURCE_DIR}/CMake")
set(SMTK_CONFIG_DIR "${PROJECT_BINARY_DIR}")
configure_file(
  ${PROJECT_SOURCE_DIR}/CMake/SMTKConfig.cmake.in
  ${PROJECT_BINARY_DIR}/SMTKConfig.cmake
  @ONLY)

################################################################################
# Create an install package configuration file
#
# Setup the config file for exports that stores what other thirdparty
# packages we need to search for ( MOAB, Remus, etc ) for when using the
# install version of smtk
set(SMTK_VTK_MODULE_DIR "\${CMAKE_CURRENT_LIST_DIR}")
set(SMTK_MODULE_DIR "\${CMAKE_CURRENT_LIST_DIR}")
set(SMTK_CONFIG_DIR "\${CMAKE_CURRENT_LIST_DIR}")
configure_file(
  ${PROJECT_SOURCE_DIR}/CMake/SMTKConfig.cmake.in
  ${PROJECT_BINARY_DIR}/CMakeFiles/SMTKConfig.cmake
  @ONLY)

install (FILES ${PROJECT_BINARY_DIR}/CMakeFiles/SMTKConfig.cmake
         DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SMTK)

################################################################################
# Build documentation
# This also includes tutorials and other documentation that has its
# source checked against SMTK libraries, so it must come after those
# targets have been declared.
################################################################################

if (SMTK_ENABLE_DOCUMENTATION)
  add_subdirectory(doc)
endif()
