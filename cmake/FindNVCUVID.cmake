# - Find Nvcuvid
# Find NVCUVID headers and libraries.
#
# NVCUVID_INCLUDE_DIRS - where to find cuviddec.h, nvcuvid.h, and cuda.h.
# NVCUVID_LIBRARIES    - List of libraries when using NVCUVID.
# NVCUVID_ROOT_DIR     - The base directory to search for NVCUVID.
#                        This can also be an environment variable.

# Look for root directory on Windows
set(NVCUVID_ROOT_DIR $ENV{NVCUVID_ROOT_DIR}
    CACHE PATH "Root directory to search for NVCUVID")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for the header file.
find_path(NVCUVID_INCLUDE_DIR
  NAMES nvcuvid.h cuviddec.h
  PATHS ${NVCUVID_ROOT_DIR}/include
        /usr/local/include
        /usr/local/cuda/include
  DOC "NVCUVID include directory"
  NO_DEFAULT_PATH)

# Look for the library.
find_library(NVCUVID_LIBRARY
  NAMES nvcuvid
  PATHS ${NVCUVID_ROOT_DIR}/lib/x64
        /usr/lib64/nvidia
        /usr/lib/x86_64-linux-gnu
  DOC "NVCUVID library"
  NO_DEFAULT_PATH)

find_library(CU_LIBRARY
  NAMES cuda
  PATHS ${NVCUVID_ROOT_DIR}/lib/x64
        /usr/lib64/nvidia
        /usr/lib/x86_64-linux-gnu
  DOC "CUDA library"
  NO_DEFAULT_PATH)

find_library(CUDART_STATIC_LIBRARY
  NAMES cudart
  PATHS ${NVCUVID_ROOT_DIR}/lib/x64
        /usr/lib/x86_64-linux-gnu
        /usr/local/cuda/lib64/
  DOC "CUDA runtime library"
  NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NVCUVID REQUIRED_VARS
                                  NVCUVID_LIBRARY
                                  CU_LIBRARY
                                  CUDART_STATIC_LIBRARY
                                  NVCUVID_INCLUDE_DIR)

if(NVCUVID_FOUND)
  set(NVCUVID_INCLUDE_DIRS ${NVCUVID_INCLUDE_DIR}
    CACHE PATH "NVCUVID include directories")

  set(NVCUVID_LIBRARIES
    ${NVCUVID_LIBRARY}
    ${CU_LIBRARY}
    ${CUDART_STATIC_LIBRARY}
    CACHE PATH "NVCUVID libraries")
endif()
