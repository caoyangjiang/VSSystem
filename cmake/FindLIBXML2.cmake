# - Find LIBXML2
# Find the LIBXML2 headers and libraries.
#
# LIBXML2_INCLUDE_DIRS - where to find LIBXML2 related headers.
# LIBXML2_LIBRARIES      - library for libxml2.
# LIBXML2_ROOT_DIR     - The base directory to search for libxml2.
#                        This can also be an environment variable.

# Look for root directory on Windows
set(LIBXML2_ROOT_DIR ${LIBXML2_ROOT_DIR} $ENV{LIBXML2_ROOT_DIR}
    CACHE PATH "Root directory to search for LibXML2")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header file
find_path(LIBXML2_INCLUDE_DIR
  NAMES libxml/xmlreader.h
  PATHS $ENV{${PFX86}}/libxml2/include
        $ENV{${PF}}/libxml2/include
        ${LIBXML2_ROOT_DIR}/include
        /usr/include/libxml2
  DOC "LibXML2 include directory"
  NO_DEFAULT_PATH)

# Look for library
find_library(LIBXML2_LIBRARY
  NAMES xml2 libxml2
  PATHS $ENV{${PFX86}}/libxml2/lib
        $ENV{${PF}}/libxml2/lib
        ${LIBXML2_ROOT_DIR}/lib
        /usr/lib/x86_64-linux-gnu/
  DOC "LibXML2 libary"
  NO_DEFAULT_PATH)

if(LIBXML2_INCLUDE_DIR)
  foreach(_libxml2_version_header libxml/xmlversion.h xmlversion.h)
    if(EXISTS "${LIBXML2_INCLUDE_DIR}/${_libxml2_version_header}")
      file(STRINGS "${LIBXML2_INCLUDE_DIR}/${_libxml2_version_header}"
        libxml2_version_str REGEX
          "^#define[\t ]+LIBXML_DOTTED_VERSION[\t ]+\".*\"")
      string(REGEX REPLACE
            "^#define[\t ]+LIBXML_DOTTED_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
            LIBXML2_VERSION_STRING "${libxml2_version_str}")
      unset(libxml2_version_str)
      break()
    endif()
  endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBXML2 REQUIRED_VARS
                                  LIBXML2_LIBRARY
                                  LIBXML2_INCLUDE_DIR
                                  VERSION_VAR
                                  LIBXML2_VERSION_STRING)

if(LIBXML2_FOUND)
  set(LIBXML2_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR}
    CACHE PATH "LibXML2 include directories")

  set(LIBXML2_LIBRARIES ${LIBXML2_LIBRARY}
    CACHE PATH "LibXML2 libraries")
endif()
