# - Find FFmpeg
# Find the native ffmpeg headers and libraries.
#
#  FFMPEG_INCLUDE_DIRS - where to find avcodec.h, etc.
#  FFMPEG_LIBRARIES    - List of libraries when using ffmpeg.
#  FFMPEG_ROOT_DIR     - The base directory to search for ffmpeg.
#                       This can also be an environment variable.

# Look for root directory on Windows
set(FFMPEG_ROOT_DIR  $ENV{FFMPEG_ROOT_DIR}
    CACHE PATH "Root directory to search for FFMPEG")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header file
find_path(FFMPEG_INCLUDE_DIR
  NAMES libavcodec/avcodec.h
  PATHS $ENV{${PFX86}}/ffmpeg/include
        $ENV{${PF}}/ffmpeg/include
        ${FFMPEG_ROOT_DIR}/include
        /usr/local/include
  DOC "FFMPEG include directory"
  NO_DEFAULT_PATH)

# Look for library
find_library(FFMPEG_AVFORMAT_LIBRARY
  NAMES avformat
  PATHS $ENV{${PFX86}}/ffmpeg/lib
        $ENV{${PF}}/ffmpeg/lib
        ${FFMPEG_ROOT_DIR}/lib
        /usr/local/lib
  DOC "AVFormat library"
  NO_DEFAULT_PATH)

# Look for library
find_library(FFMPEG_AVUTIL_LIBRARY
  NAMES avutil
  PATHS $ENV{${PFX86}}/ffmpeg/lib
        $ENV{${PF}}/ffmpeg/lib
        ${FFMPEG_ROOT_DIR}/lib
        /usr/local/lib
  DOC "AVUtil library"
  NO_DEFAULT_PATH)

# Look for library
find_library(FFMPEG_AVCODEC_LIBRARY
  NAMES avcodec
  PATHS $ENV{${PFX86}}/ffmpeg/lib
        $ENV{${PF}}/ffmpeg/lib
        ${FFMPEG_ROOT_DIR}/lib
        /usr/local/lib
  DOC "AVcodec library"
  NO_DEFAULT_PATH)

# Find major version (optional)
if(FFMPEG_INCLUDE_DIR)
  foreach(_libavcodec_version_header libavcodec/version.h)
    if(EXISTS "${FFMPEG_INCLUDE_DIR}/${_libavcodec_version_header}")
      file(STRINGS "${FFMPEG_INCLUDE_DIR}/${_libavcodec_version_header}"
        libavcodec_version_str REGEX
        "^#define[\t ]+LIBAVCODEC_VERSION_MAJOR[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+LIBAVCODEC_VERSION_MAJOR[\t ]+([0-9]*)" "\\1"
        FFMPEG_MAJOR_VERSION "${libavcodec_version_str}")
      unset(libavcodec_version_str)
      break()
    endif()
  endforeach()
endif()

# Find minor version (optional)
if(FFMPEG_INCLUDE_DIR)
  foreach(_libavcodec_version_header libavcodec/version.h)
    if(EXISTS "${FFMPEG_INCLUDE_DIR}/${_libavcodec_version_header}")
      file(STRINGS "${FFMPEG_INCLUDE_DIR}/${_libavcodec_version_header}"
        libavcodec_version_str REGEX
        "^#define[\t ]+LIBAVCODEC_VERSION_MINOR[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+LIBAVCODEC_VERSION_MINOR[\t ]+([0-9]*)" "\\1"
        FFMPEG_MINOR_VERSION "${libavcodec_version_str}")
      unset(libavcodec_version_str)
      break()
    endif()
  endforeach()
endif()

# Merge verion string (optional)
string(CONCAT FFMPEG_VERSION_STRING
  ${FFMPEG_MAJOR_VERSION}
  "."
  ${FFMPEG_MINOR_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFMPEG REQUIRED_VARS
                                  FFMPEG_AVCODEC_LIBRARY
                                  FFMPEG_INCLUDE_DIR
                                  FFMPEG_AVFORMAT_LIBRARY
                                  FFMPEG_AVUTIL_LIBRARY
                                  VERSION_VAR
                                  FFMPEG_VERSION_STRING)

if(FFMPEG_FOUND)
  set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR}
    CACHE PATH "FFMPEG include directories")

  set(FFMPEG_LIBRARIES
    ${FFMPEG_AVFORMAT_LIBRARY}
    ${FFMPEG_AVUTIL_LIBRARY}
    ${FFMPEG_AVCODEC_LIBRARY}
    CACHE PATH "FFMPEG libraries")
endif()
