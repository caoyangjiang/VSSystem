cmake_minimum_required(VERSION 3.7)
project(jcy_rtpserver)

if(MSVC)
  set(DLLEXPORT "__declspec(dllexport)")
  set(DISABLE "__pragma(warning(push,0))")
  set(ENABLE "__pragma(warning(pop))")
endif()

set(PROJECT_SRCS
${PROJECT_SOURCE_DIR}/src/RTPTime.cpp
${PROJECT_SOURCE_DIR}/src/RTPPacket.cpp
${PROJECT_SOURCE_DIR}/src/Nalu.cpp
${PROJECT_SOURCE_DIR}/src/RTPH264Builder.cpp
${PROJECT_SOURCE_DIR}/src/RTPSession.cpp

)

set(PROJECT_INCS
${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/RTPTime.h
${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/RTPPacket.h
${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/Nalu.h
${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/RTPH264Builder.h
${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/RTPSession.h
)

source_group("include" FILES ${PROJECT_INCS})
source_group("src"     FILES ${PROJECT_SRCS})

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS} ${PROJECT_INCS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
                          JCY_WINDOWS_DISABLE_ALL_WARNING=${DISABLE}
                          JCY_WINDOWS_ENABLE_ALL_WARNING=${ENABLE})

target_compile_definitions(${PROJECT_NAME} INTERFACE
                          JCY_RTPSERVER_DLL=${DLLIMPORT})

target_compile_definitions(${PROJECT_NAME} PRIVATE
                          JCY_RTPSERVER_DLL=${DLLEXPORT})

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

install(FILES
  ${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/RTPH264Builder.h
  ${PROJECT_SOURCE_DIR}/include/Jcy/RTPServer/RTPPacket.h
  DESTINATION include/Jcy/RTPServer)

export(TARGETS
        ${PROJECT_NAME}
        APPEND FILE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Targets.cmake")

file(APPEND
    ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    "set(JCY_CODEC_LIBRARIES \${JCY_CODEC_LIBRARIES} ${PROJECT_NAME})\n")