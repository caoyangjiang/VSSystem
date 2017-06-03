cmake_minimum_required(VERSION 3.7)
project(jcy_gpuencoder)

if(MSVC)
  set(DLLEXPORT "__declspec(dllexport)")
  set(DISABLE "__pragma(warning(push,0))")
  set(ENABLE "__pragma(warning(pop))")
endif()


set(PROJECT_SRCS
${PROJECT_SOURCE_DIR}/src/GpuVideoEncoder.cpp
)

set(PROJECT_INCS
${PROJECT_SOURCE_DIR}/include/Jcy/GpuEncoder/GpuVideoEncoder.h
)

source_group("include" FILES ${PROJECT_INCS})
source_group("src"     FILES ${PROJECT_SRCS})

find_package( CUDA 7.5 REQUIRED )
find_package( NVENCODE 7.0 REQUIRED )

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS} ${PROJECT_INCS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
  JCY_WINDOWS_DISABLE_ALL_WARNING=${DISABLE}
  JCY_WINDOWS_ENABLE_ALL_WARNING=${ENABLE})

target_compile_definitions(${PROJECT_NAME} INTERFACE
  JCY_GPUENCODER_DLL=${DLLIMPORT})

target_compile_definitions(${PROJECT_NAME} PRIVATE
  JCY_GPUENCODER_DLL=${DLLEXPORT})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
                                      ${CUDA_INCLUDE_DIRS}
                                      ${NVENCODE_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
                            ${CUDA_LIBRARIES})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${NVENCODE_LIBRARIES})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY
  FOLDER ${CMAKE_PROJECT_NAME}/modules)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(FILES
  ${PROJECT_SOURCE_DIR}/include/Jcy/GpuEncoder/GpuVideoEncoder.h
  DESTINATION include/Jcy/GpuEncoder)

export(TARGETS
      ${PROJECT_NAME}
      APPEND FILE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake")

file(APPEND
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    "set(JCY_VSSYSTEM_LIBRARIES \${JCY_VSSYSTEM_LIBRARIES} ${PROJECT_NAME})\n")