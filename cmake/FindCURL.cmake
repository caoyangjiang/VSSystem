# - Find CURL
# Find the native CURL headers and libraries.
#
# CURL_INCLUDE_DIRS   - where to find curl/curl.h, etc.
# CURL_LIBRARIES      - List of libraries when using curl.
# CURL_FOUND          - True if curl found.
# CURL_VERSION_STRING - the version of curl found

# Look for root directory on Windows
set(CURL_ROOT_DIR $ENV{CURL_ROOT_DIR}
    CACHE PATH "Root directory to search for libcurl")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header file
find_path(CURL_INCLUDE_DIR
  NAMES curl/curl.h
  PATHS $ENV{${PFX86}}/CURL/include
        $ENV{${PF}}/CURL/include
        ${CURL_ROOT_DIR}/include
        /usr/local/include
        /usr/include
  DOC "CURL include directory"
  NO_DEFAULT_PATH)

# Look for the library
find_library(CURL_LIBRARY
  NAMES curl curllib libcurl_imp libcurl
  PATHS $ENV{${PFX86}}/CURL/lib
        $ENV{${PF}}/CURL/lib
        ${CURL_ROOT_DIR}/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
  DOC "CURL library"
  NO_DEFAULT_PATH)

if(CURL_INCLUDE_DIR)
  foreach(_curl_version_header curlver.h curl.h)
    if(EXISTS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}")
      file(STRINGS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}"
          curl_version_str REGEX "^#define[\t ]+LIBCURL_VERSION[\t ]+\".*\"")
      string(REGEX REPLACE
            "^#define[\t ]+LIBCURL_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
            CURL_VERSION_STRING "${curl_version_str}")
      unset(curl_version_str)
      break()
    endif()
  endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CURL
                                  REQUIRED_VARS
                                  CURL_LIBRARY
                                  CURL_INCLUDE_DIR
                                  VERSION_VAR
                                  CURL_VERSION_STRING)

if(CURL_FOUND)
  set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR}
    CACHE PATH "CURL include directories")

  set(CURL_LIBRARIES ${CURL_LIBRARY}
    CACHE PATH "CURL libraries")
endif()