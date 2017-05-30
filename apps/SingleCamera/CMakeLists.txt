cmake_minimum_required(VERSION 3.7)
project(SingleCamera)

set(PROJECT_SRCS
${PROJECT_SOURCE_DIR}/src/SingleCamera.cpp
)

source_group("src" FILES ${PROJECT_SRCS})

add_executable(${PROJECT_NAME} ${PROJECT_SRCS})

find_package( Boost 1.58 COMPONENTS system REQUIRED )

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
                          ${Boost_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
                              ${Boost_LIBRARIES}
                              jcy_camera
                              jcy_cpuencoder
                              jcy_gpuencoder
                              jcy_rtpserver)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${CMAKE_PROJECT_NAME}/apps)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        DESTINATION bin)
