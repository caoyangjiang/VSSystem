cmake_minimum_required(VERSION 3.7)
project(hvr_cudasample)

if(MSVC)
  set(DLLIMPORT "__declspec(dllimport)")
  set(DLLEXPORT "__declspec(dllexport)")
endif()

set(PROJECT_SRCS
${PROJECT_SOURCE_DIR}/src/CUDAConfig.cpp
${PROJECT_SOURCE_DIR}/src/CUDASample.cu
)

set(PROJECT_INCS
${PROJECT_SOURCE_DIR}/include/Hvr/CUDASample/CUDAConfig.h
${PROJECT_SOURCE_DIR}/include/Hvr/CUDASample/CUDASample.cuh
)

source_group("include" FILES ${PROJECT_INCS})
source_group("src"     FILES ${PROJECT_SRCS})

find_package( CUDA REQUIRED )
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR CMAKE_COMPILER_IS_GNUCC
    OR CMAKE_COMPILER_IS_GNUCXX)
  set(CUDA_PROPAGATE_HOST_FLAGS OFF)
  set(CUDA_NVCC_FLAGS "-std=c++11")
endif()
list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_61,code=sm_61")
list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_52,code=sm_52")
list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_50,code=sm_50")
list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_35,code=sm_35")
list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_30,code=sm_30")

# for visual studio
string(REPLACE "/w44668" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "/sdl" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

cuda_add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS} ${PROJECT_INCS})

target_compile_definitions(${PROJECT_NAME} INTERFACE
                          HVR_CUDASAMPLE_DLL=${DLLIMPORT})

target_compile_definitions(${PROJECT_NAME} PRIVATE
                          HVR_CUDASAMPLE_DLL=${DLLEXPORT})

target_include_directories(${PROJECT_NAME} PUBLIC
                          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

set_property(TARGET ${PROJECT_NAME} PROPERTY
            FOLDER ${CMAKE_PROJECT_NAME}/modules)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES ${PROJECT_SOURCE_DIR}/include/Hvr/CUDASample/CUDASample.cuh
              ${PROJECT_SOURCE_DIR}/include/Hvr/CUDASample/CUDAConfig.h
        DESTINATION include/Hvr/CUDASample)

export(TARGETS ${PROJECT_NAME}
      APPEND FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake)

file(APPEND
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    "set(HVR_SAMPLE_LIBRARIES \${HVR_SAMPLE_LIBRARIES} ${PROJECT_NAME})\n")
