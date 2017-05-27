# - Find Rapture3D Universal SDK
# Find the native Rapturee3D universal SDK headers and libraries.
# This is currently Windows-only library.
#
# RAPTURE3D_INCLUDE_DIRS - where to find R3D/r3d.h, etc.
# RAPTURE3D_LIBRARIES    - where to find R3DU.dll.
# RAPTURE3D_ROOT_DIR    - The base directory to search for Rapture3D.
#                         This can also be an environment variable.

# Look for root directory on Windows
set(RAPTURE3D_ROOT_DIR $ENV{RAPTURE3D_ROOT_DIR}
  CACHE PATH "Root directory to search for Rapture3D")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header file.
find_path(RAPTURE3D_INCLUDE_DIR
  NAMES R3D/r3d.h
  PATHS $ENV{${PFX86}}/Rapture3D/include
        $ENV{${PF}}/Rapture3D/include
        ${RAPTURE3D_ROOT_DIR}/include
        /usr/local/include/
  DOC "Rapture3D include directory"
  NO_DEFAULT_PATH)

# Look for library.
find_library(RAPTURE3D_LIBRARY
  NAMES R3DU
  PATHS $ENV{${PFX86}}/Rapture3D/lib
        $ENV{${PF}}/Rapture3D/lib
        ${RAPTURE3D_ROOT_DIR}/lib/x64
        /usr/local/lib/
  DOC "Rapture3D library"
  NO_DEFAULT_PATH)

# Find major version (optional)
if(RAPTURE3D_INCLUDE_DIR)
  foreach(_rapture3d_version_header R3D/r3d.h)
    if(EXISTS "${RAPTURE3D_INCLUDE_DIR}/${_rapture3d_version_header}")
      file(STRINGS "${RAPTURE3D_INCLUDE_DIR}/${_rapture3d_version_header}"
        rapture3d_version_str REGEX
        "^#define[\t ]+R3D_MAJOR_VERSION[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+R3D_MAJOR_VERSION[\t ]+([0-9]*)" "\\1"
        RAPTURE3D_MAJOR_VERSION "${rapture3d_version_str}")
      unset(rapture3d_version_str)
      break()
    endif()
  endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RAPTURE3D REQUIRED_VARS
                                  RAPTURE3D_LIBRARY
                                  RAPTURE3D_INCLUDE_DIR
                                  VERSION_VAR
                                  RAPTURE3D_MAJOR_VERSION)

if(RAPTURE3D_FOUND)
  set(RAPTURE3D_INCLUDE_DIRS ${RAPTURE3D_INCLUDE_DIR}
    CACHE PATH "Rapture3D include directories")

  set(RAPTURE3D_LIBRARIES ${RAPTURE3D_LIBRARY}
    CACHE PATH "Rapture3D libraries")
endif()
