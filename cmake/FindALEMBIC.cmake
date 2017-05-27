# - Find Alembic
# Find the Alembic headers and libraries.
#
# ALEMBIC_INCLUDE_DIRS - where to find alembic and ilmbase related headers.
# ALEMBIC_LIBRARIES      - library for Alembic.
# ALEMBIC_ROOT_DIR     - The base directory to search for Alembic.
#                        This can also be an environment variable.

# Look for root directory on Windows
set(ALEMBIC_ROOT_DIR $ENV{ALEMBIC_ROOT_DIR}
  CACHE PATH "Root directory to search for Alembic")
set(ALEMBIC_ROOT_DIR $ENV{ILMBASE_ROOT_DIR}
  CACHE PATH "Root directory to search for ilmbase")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header files.
find_path(ALEMBIC_INCLUDE_DIR
  NAMES Alembic/AbcGeom/Foundation.h
  PATHS $ENV{${PFX86}}/Alembic/include
        $ENV{${PF}}/Alembic/include
        ${ALEMBIC_ROOT_DIR}/include
        /usr/local/include
  DOC "Alembic include directory"
  NO_DEFAULT_PATH)

# Look for header files
find_path(ILMBASE_INCLUDE_DIR
  NAMES half.h
  PATHS $ENV{${PFX86}}/ilmbase/include/OpenEXR
        $ENV{${PF}}/ilmbase/include/OpenEXR
        ${ILMBASE_ROOT_DIR}/include/OpenEXR
        /usr/local/include/OpenEXR
  DOC "ilmbase include directory"
  NO_DEFAULT_PATH)

# Look for the library.
find_library(ALEMBIC_LIBRARY
  NAMES Alembic
  PATHS $ENV{${PFX86}}/Alembic/lib
        $ENV{${PF}}/Alembic/lib
        ${ALEMBIC_ROOT_DIR}/lib
        /usr/local/lib
  DOC "Almebic library"
  NO_DEFAULT_PATH)

# Find version (optional)
if(ALEMBIC_INCLUDE_DIR)
  foreach(_alembic_version_header Alembic/Util/Config.h)
    if(EXISTS "${ALEMBIC_INCLUDE_DIR}/${_alembic_version_header}")
      string(CONCAT majorstr "^#define[\\t ]ALEMBIC_LIBRARY_VERSION[\\t ]"
        "([0-9])[\t ].[\t ][0-9]+[\t ].[\t ][0-9].*")
      string(CONCAT minorstr "^#define[\\t ]ALEMBIC_LIBRARY_VERSION[\\t ]"
        "[0-9][\t ].[\t ][0-9]+[\t ].[\t ]([0-9]).*")

      file(STRINGS "${ALEMBIC_INCLUDE_DIR}/${_alembic_version_header}"
          alembic_version_str REGEX "^#define[\t ]ALEMBIC_LIBRARY_VERSION.*")
      string(REGEX REPLACE
            ${majorstr} "\\1"
            ALEMBIC_MAJOR_VERSION "${alembic_version_str}")
      string(REGEX REPLACE
            ${minorstr} "\\1"
            ALEMBIC_MINOR_VERSION "${alembic_version_str}")
      unset(alembic_version_str)
      break()
    endif()
  endforeach()
endif()

# Merge verion string (optional)
string(CONCAT ALEMBIC_VERSION_STRING
  ${ALEMBIC_MAJOR_VERSION}
  "."
  ${ALEMBIC_MINOR_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ALEMBIC
                                  REQUIRED_VARS
                                  ALEMBIC_LIBRARY
                                  ALEMBIC_INCLUDE_DIR
                                  ILMBASE_INCLUDE_DIR
                                  VERSION_VAR
                                  ALEMBIC_VERSION_STRING)
if(ALEMBIC_FOUND)
  set(ALEMBIC_INCLUDE_DIRS
    ${ALEMBIC_INCLUDE_DIR}
    ${ILMBASE_INCLUDE_DIR}
    CACHE PATH "Alembic include directories")

  set(ALEMBIC_LIBRARIES ${ALEMBIC_LIBRARY}
    CACHE PATH "Alembic libraries")
endif()