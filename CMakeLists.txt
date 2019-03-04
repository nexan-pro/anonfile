cmake_minimum_required(VERSION 3.9)
project(anonfile)

set(CMAKE_CXX_STANDARD 17)
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

add_executable(anonfile main.cpp helpers/helpers.cpp helpers/helpers.h pjson/pjson.h json_handler/JHandler.cpp json_handler/JHandler.h)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(anonfile ${CURL_LIBRARIES})
