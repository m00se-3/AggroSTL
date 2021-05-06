cmake_minimum_required(VERSION 3.12)

project(AggroSTL LANGUAGES CXX)

include(CTest)

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  list(APPEND flags "-W4")
elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
   list(APPEND flags "-Wall -Wextra -Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
   list(APPEND flags "-Wall -Wextra" "-Wpedantic")
endif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)

add_library(aggrostl INTERFACE)
add_executable(arrays)
include(${CMAKE_CURRENT_LIST_DIR}/AggroSTL/CMakeLists.txt)

target_compile_features(aggrostl INTERFACE cxx_std_20)
target_compile_options(aggrostl INTERFACE ${flags})
set_target_properties(aggrostl PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(arrays PRIVATE cxx_std_20)
target_compile_options(arrays PRIVATE ${flags})
target_include_directories(arrays PRIVATE ${CMAKE_CURRENT_LIST_DIR}/AggroSTL/aggro/)
target_link_libraries(arrays PRIVATE aggrostl)

add_test(NAME arraytest COMMAND arrays)