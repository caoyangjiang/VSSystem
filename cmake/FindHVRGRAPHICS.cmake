# - Find HVRGRAPHICS
# Find the HVRgraphics headers and libraries.
#
#  HVRGRAPHICS_INCLUDE_DIRS - where to find HVRGRAPHICS related headers.
#  HVRGRAPHICS_LIBRARY      - library for HVRGRAPHICS.
#  HVRGRAPHICS_ROOT_DIR     - The base directory to search for hvr-graphics.
#                             This can also be an environment variable.

# Look for root directory on Windows
set(HVRGRAPHICS_ROOT_DIR  $ENV{HVRGRAPHICS_ROOT_DIR}
    CACHE PATH "Root directory to search for hvr-graphics")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")


# Look for header file
find_path(HVRGRAPHICS_INCLUDE_DIR
  NAMES Triangulator2D.h
  PATHS $ENV{${PFX86}}/hvr-graphics/include
        $ENV{${PF}}/hvr-graphics/include
        ${HVRGRAPHICS_ROOT_DIR}/include
        /usr/local/include
  DOC "hvr-graphics include directory"
  NO_DEFAULT_PATH)

# Look for library.
find_library(HVRGRAPHICS_LIBRARY
  NAMES hvr_triangulator2D
  PATHS $ENV{${PFX86}}/hvr-graphics/lib
        $ENV{${PF}}/hvr-graphics/lib
        ${HVRGRAPHICS_ROOT_DIR}/lib
        /usr/local/lib
  DOC "hvr-graphics library"
  NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HVRGRAPHICS REQUIRED_VARS
                                  HVRGRAPHICS_LIBRARY HVRGRAPHICS_INCLUDE_DIR)

if(HVRGRAPHICS_FOUND)
  set(HVRGRAPHICS_INCLUDE_DIRS ${HVRGRAPHICS_INCLUDE_DIR}
    CACHE PATH "hvr-graphics include directories")

  set(HVRGRAPHICS_LIBRARIES ${HVRGRAPHICS_LIBRARY}
    CACHE PATH "hvr-graphics libraries")
endif()