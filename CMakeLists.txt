cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(node-samp-plugin)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/deps/cmake")
include(AddSAMPPlugin)
set(CMAKE_CXX_STANDARD 17)
include_directories(
    "${PROJECT_SOURCE_DIR}/deps/samp-plugin-sdk/src"
    "${PROJECT_SOURCE_DIR}/deps/sampgdk/src"
    "${PROJECT_SOURCE_DIR}/deps/node/include" 
    "${PROJECT_SOURCE_DIR}/deps/uv/include" 
    "${PROJECT_SOURCE_DIR}/deps/v8/include"
    "${PROJECT_SOURCE_DIR}/deps/iconv-lite/src"
    "${PROJECT_SOURCE_DIR}/deps/rang/include"
    "${PROJECT_SOURCE_DIR}/deps/json/include"
)
file(
    GLOB CPPS 
    "${PROJECT_SOURCE_DIR}/deps/samp-plugin-sdk/src/amx/*.c" 
    "${PROJECT_SOURCE_DIR}/deps/samp-plugin-sdk/src/*.cpp" 
    "${PROJECT_SOURCE_DIR}/deps/sampgdk/src/*.c"
    "${PROJECT_SOURCE_DIR}/deps/iconv-lite/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
if(UNIX)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH true)
endif()
add_samp_plugin(nodesamp ${CPPS} plugin.def)
if(WIN32)
    target_link_libraries(nodesamp PUBLIC "${PROJECT_SOURCE_DIR}/deps/node/lib/libnode.lib")
else()
    set_target_properties(nodesamp PROPERTIES INSTALL_RPATH "$ORIGIN/..:$ORIGIN/")
    target_link_libraries(nodesamp PUBLIC "${PROJECT_SOURCE_DIR}/deps/node/lib/libnode.so.93")
endif()