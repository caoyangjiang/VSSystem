cmake_minimum_required(VERSION 3.7)
project(jcy_dip)

if(MSVC)
  set(DLLEXPORT "__declspec(dllexport)")
  set(DISABLE "__pragma(warning(push,0))")
  set(ENABLE "__pragma(warning(pop))")
endif()


set(PROJECT_SRCS
${PROJECT_SOURCE_DIR}/src/Tools.cpp
)

set(PROJECT_INCS
${PROJECT_SOURCE_DIR}/include/Jcy/DIP/Tools.h
)

source_group("include" FILES ${PROJECT_INCS})
source_group("src"     FILES ${PROJECT_SRCS})

find_package( OpenCV REQUIRED )

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS} ${PROJECT_INCS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
  JCY_WINDOWS_DISABLE_ALL_WARNING=${DISABLE}
  JCY_WINDOWS_ENABLE_ALL_WARNING=${ENABLE})

target_compile_definitions(${PROJECT_NAME} INTERFACE
  JCY_DIP_DLL=${DLLIMPORT})

target_compile_definitions(${PROJECT_NAME} PRIVATE
  JCY_DIP_DLL=${DLLEXPORT})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
                                      ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
                            ${OpenCV_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY
  FOLDER ${CMAKE_PROJECT_NAME}/modules)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(FILES
  ${PROJECT_SOURCE_DIR}/include/Jcy/DIP/Tools.h
  DESTINATION include/Jcy/DIP)

export(TARGETS
      ${PROJECT_NAME}
      APPEND FILE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake")

file(APPEND
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    "set(JCY_VSSYSTEM_LIBRARIES \${JCY_VSSYSTEM_LIBRARIES} ${PROJECT_NAME})\n")