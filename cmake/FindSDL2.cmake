# - Find SDL2
# Find the native SDL2 headers and libraries.
#
# SDL2_INCLUDE_DIRS - where to find SDL.h, etc.
# SDL2_LIBRARIES    - List of libraries when using SDL2.
# SDL2_ROOT_DIR     - The base directory to search for SDL2.
#                     This can also be an environment variable.

# Look for root directory on Windows
set(SDL2_ROOT_DIR $ENV{SDL2_ROOT_DIR}
    CACHE PATH "Root directory to search for SDL2")
set(PFX86 "PROGRAMFILES(X86)")
set(PF    "PROGRAMFILES")

# Look for header file
find_path(SDL2_INCLUDE_DIR
  NAMES SDL.h
  PATHS $ENV{${PFX86}}/SDL2/include
        $ENV{${PF}}/SDL2/include
        ${SDL2_ROOT_DIR}/include
        /usr/include/SDL2
        /usr/local/include/SDL2
  DOC "SDL2 include directory"
  NO_DEFAULT_PATH)

# Look for library
find_library(SDL2_LIBRARY
  NAMES SDL2
  PATHS $ENV{${PFX86}}/SDL2/lib
        $ENV{${PF}}/SDL2/lib
        ${SDL2_ROOT_DIR}/lib/x64
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
  DOC "SDL2 libary"
  NO_DEFAULT_PATH)

# Look for library
find_library(SDL2_MAIN_LIBRARY
  NAMES SDL2main
  PATHS $ENV{${PFX86}}/SDL2/lib
        $ENV{${PF}}/SDL2/lib
        ${SDL2_ROOT_DIR}/lib/x64
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
  DOC "SDL2 main libary"
  NO_DEFAULT_PATH)

# Find major version (optional)
if(SDL2_INCLUDE_DIR)
  foreach(_sdl2_version_header SDL_version.h)
    if(EXISTS "${SDL2_INCLUDE_DIR}/${_sdl2_version_header}")
      file(STRINGS "${SDL2_INCLUDE_DIR}/${_sdl2_version_header}"
        sdl2_version_str REGEX
        "^#define[\t ]+SDL_MAJOR_VERSION[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+SDL_MAJOR_VERSION[\t ]+([0-9]*)" "\\1"
        SDL2_MAJOR_VERSION "${sdl2_version_str}")
      unset(sdl2_version_str)
      break()
    endif()
  endforeach()
endif()

# Find minor version (optional)
if(SDL2_INCLUDE_DIR)
  foreach(_sdl2_version_header SDL_version.h)
    if(EXISTS "${SDL2_INCLUDE_DIR}/${_sdl2_version_header}")
      file(STRINGS "${SDL2_INCLUDE_DIR}/${_sdl2_version_header}"
        sdl2_version_str REGEX
        "^#define[\t ]+SDL_MINOR_VERSION[\t ]+[0-9]")
      string(REGEX REPLACE
        "^#define[\t ]+SDL_MINOR_VERSION[\t ]+([0-9]*)" "\\1"
        SDL2_MINOR_VERSION "${sdl2_version_str}")
      unset(sdl2_version_str)
      break()
    endif()
  endforeach()
endif()

# Merge verion string (optional)
string(CONCAT SDL2_VERSION_STRING
  ${SDL2_MAJOR_VERSION}
  "."
  ${SDL2_MINOR_VERSION})


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS
                                  SDL2_LIBRARY
                                  SDL2_MAIN_LIBRARY
                                  SDL2_INCLUDE_DIR
                                  VERSION_VAR
                                  SDL2_VERSION_STRING)

if(SDL2_FOUND)
  set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR}
    CACHE PATH "SDL2 include directories")

  set(SDL2_LIBRARIES ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY}
    CACHE PATH "SDL2 libraries")
endif()
