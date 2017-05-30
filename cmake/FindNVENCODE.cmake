# - Find NVCodec
# Find NVCodec headers and libraries.
#
# NVENCODE_INCLUDE_DIRS - where to find cuda.h and nvEncodeAPI.h
# NVENCODE_LIBRARIES    - List of libraries when using NVENCODE
# NVENCODE_ROOT_DIR     - The base directory to search for NVENCODE.
#                        This can also be an environment variable.

# Look for root directory on Windows
set(NVENCODE_ROOT_DIR $ENV{NVENCODE_ROOT_DIR}
  CACHE PATH "Root directory to search for NVENCODE")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for the header file.
find_path(NVENCODE_INCLUDE_DIR
  NAMES nvEncodeAPI.h
  PATHS ${NVENCODE_ROOT_DIR}/
        /usr/local/include
        /usr/local/include/NVCodec
  DOC "NVENCODE include directory"
  NO_DEFAULT_PATH)

# Look for library
find_library(NVENCODE_LIBRARY
  NAMES nvidia-encode
  PATHS /usr/lib64/nvidia
        /usr/lib/x86_64-linux-gnu
  DOC "NVENCODE library")

# Look for library
# find_library(CU_LIBRARY
#   NAMES cuda
#   PATHS ${NVCUVID_ROOT_DIR}/lib/x64
#         ${NVENCODE_ROOT_DIR}/lib/x64
#         /usr/lib/x86_64-linux-gnu
#   DOC "CUDA library"
#   NO_DEFAULT_PATH)

find_library(CUDART_STATIC_LIBRARY
  NAMES cudart
  PATHS ${NVCUVID_ROOT_DIR}/lib/x64
        ${NVENCODE_ROOT_DIR}/lib/x64
        /usr/lib/x86_64-linux-gnu
        /usr/local/cuda/lib64/
  DOC "CUDA runtime library"
  NO_DEFAULT_PATH)

# Find major version (optional)
if(NVENCODE_INCLUDE_DIR)
  foreach(_nvencode_version_header nvEncodeAPI.h)
    if(EXISTS "${NVENCODE_INCLUDE_DIR}/${_nvencode_version_header}")
      file(STRINGS "${NVENCODE_INCLUDE_DIR}/${_nvencode_version_header}"
        nvencode_version_str REGEX
        "^#define[\t ]+NVENCAPI_MAJOR_VERSION[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+NVENCAPI_MAJOR_VERSION[\t ]+([0-9]*)" "\\1"
        NVENCODE_MAJOR_VERSION "${nvencode_version_str}")
      unset(nvencode_version_str)
      break()
    endif()
  endforeach()
endif()

# Find minor version (optional)
if(NVENCODE_INCLUDE_DIR)
  foreach(_nvencode_version_header nvEncodeAPI.h)
    if(EXISTS "${NVENCODE_INCLUDE_DIR}/${_nvencode_version_header}")
      file(STRINGS "${NVENCODE_INCLUDE_DIR}/${_nvencode_version_header}"
        nvencode_version_str REGEX
        "^#define[\t ]+NVENCAPI_MINOR_VERSION[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+NVENCAPI_MINOR_VERSION[\t ]+([0-9]*)" "\\1"
        NVENCODE_MINOR_VERSION "${nvencode_version_str}")
      unset(nvencode_version_str)
      break()
    endif()
  endforeach()
endif()

# Merge verion string (optional)
string(CONCAT NVENCODE_VERSION_STRING
  ${NVENCODE_MAJOR_VERSION}
  "."
  ${NVENCODE_MINOR_VERSION})


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NVENCODE REQUIRED_VARS
                                  NVENCODE_INCLUDE_DIR
                                  NVENCODE_LIBRARY
                                  #CU_LIBRARY
                                  CUDART_STATIC_LIBRARY
                                  VERSION_VAR
                                  NVENCODE_VERSION_STRING)

if(NVENCODE_FOUND)
  set(NVENCODE_INCLUDE_DIRS
    ${NVENCODE_INCLUDE_DIR}
    CACHE PATH "NVENCODE include directories")

  set(NVENCODE_LIBRARIES
    ${NVENCODE_LIBRARY}
    #${CU_LIBRARY}
    ${CUDART_STATIC_LIBRARY}
    CACHE PATH "NVENCODE libraries")
endif()