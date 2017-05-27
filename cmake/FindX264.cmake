# - Find X264
# Find X264 headers and libraries.
#
# X264_INCLUDE_DIRS - where to find x264.h, etc.
# X264_LIBRARIES    - List of libraries when using x264.
# X264_FOUND        - True if x264 found.

# Look for root directory on Windows
set(X264_ROOT_DIR ${X264_ROOT_DIR} $ENV{X264_ROOT_DIR}
  CACHE PATH "Root directory to search for X264")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for the header files.
find_path(X264_INCLUDE_DIR
  NAMES x264.h
  PATHS $ENV{${PFX86}}/x264/include
        $ENV{${PF}}/x264/include
        ${X264_ROOT_DIR}/include
        /usr/local/include
        /usr/include
  DOC "X264 include directory"
  NO_DEFAULT_PATH)

# Look for the library.
find_library(X264_LIBRARY
  NAMES x264 libx264
  PATHS $ENV{${PFX86}}/x264/lib
        $ENV{${PF}}/x264/lib
        ${X264_ROOT_DIR}/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu/
  DOC "X264 library"
  NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(X264 REQUIRED_VARS
                                  X264_LIBRARY
                                  X264_INCLUDE_DIR)

if(X264_FOUND)
  set(X264_INCLUDE_DIRS ${X264_INCLUDE_DIR}
    CACHE PATH "X264 include directories")

  set(X264_LIBRARIES ${X264_LIBRARY}
    CACHE PATH "X264 libraries")
endif()