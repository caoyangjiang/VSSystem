# - Find GLEW
# Find the GLEW headers and libraries.
#
# GLEW_INCLUDE_DIRS - where to find GLEW related headers.
# GLEW_LIBRARIES    - library for GLEW.
# GLEW_ROOT_DIR     - The base directory to search for GLEW.
#                     This can also be an environment variable.

# Look for root directory on Windows
set(GLEW_ROOT_DIR  $ENV{GLEW_ROOT_DIR}
    CACHE PATH "Root directory to search for GLEW")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header file
find_path(GLEW_INCLUDE_DIR
  NAMES GL/glew.h
  PATHS $ENV{${PFX86}}/GLEW/include
        $ENV{${PF}}/GLEW/include
        ${GLEW_ROOT_DIR}/GLEW/include
        ${GLEW_ROOT_DIR}/include
        /usr/include
        /usr/local/include
  DOC "GLEW include path"
  NO_DEFAULT_PATH)

# Look for library
find_library(GLEW_LIBRARY
  NAMES glew GLEW glew32 glew32s
  PATHS $ENV{${PFX86}}/GLEW/lib
        $ENV{${PF}}/GLEW/lib
        ${GLEW_ROOT_DIR}/GLEW/lib
        ${GLEW_ROOT_DIR}/lib/Release/x64
        /usr/lib
        /usr/local/lib
  DOC "The GLEW library"
  NO_DEFAULT_PATH)

# Find major version (optional)
if(GLEW_INCLUDE_DIR)
  foreach(_glew_version_header GL/glew.h)
    if(EXISTS "${GLEW_INCLUDE_DIR}/${_glew_version_header}")
      file(STRINGS "${GLEW_INCLUDE_DIR}/${_glew_version_header}"
        glew_version_str REGEX
        "^#define[\t ]+GLEW_VERSION_MAJOR[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+GLEW_VERSION_MAJOR[\t ]+([0-9]*)" "\\1"
        GLEW_MAJOR_VERSION "${glew_version_str}")
      unset(glew_version_str)
      break()
    endif()
  endforeach()
endif()

# Find minor version (optional)
if(GLEW_INCLUDE_DIR)
  foreach(_glew_version_header GL/glew.h)
    if(EXISTS "${GLEW_INCLUDE_DIR}/${_glew_version_header}")
      file(STRINGS "${GLEW_INCLUDE_DIR}/${_glew_version_header}"
        glew_version_str REGEX
        "^#define[\t ]+GLEW_VERSION_MINOR[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+GLEW_VERSION_MINOR[\t ]+([0-9]*)" "\\1"
        GLEW_MINOR_VERSION "${glew_version_str}")
      unset(glew_version_str)
      break()
    endif()
  endforeach()
endif()

# Merge verion string (optional)
string(CONCAT GLEW_VERSION_STRING
  ${GLEW_MAJOR_VERSION}
  "."
  ${GLEW_MINOR_VERSION})


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW REQUIRED_VARS
                                  GLEW_INCLUDE_DIR
                                  GLEW_LIBRARY
                                  VERSION_VAR
                                  GLEW_VERSION_STRING)

if(GLEW_FOUND)
  set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR}
    CACHE PATH "GLEW include directories")

  set(GLEW_LIBRARIES ${GLEW_LIBRARY}
    CACHE PATH "GLEW libraries")
endif()