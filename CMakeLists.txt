cmake_minimum_required(VERSION 3.12)

project(AggroSTL LANGUAGES CXX)

include(CTest)

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  list(APPEND flags "-W4")
elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
   list(APPEND flags "-Wall" "-Wextra" "-Wpedantic" "-fconcept")
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
   list(APPEND flags "-Wall" "-Wextra" "-Wpedantic" "-Wno-newline-eof" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic")
endif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)

add_library(aggrostl INTERFACE)
add_executable(arrays)
add_executable(lists)
include(${CMAKE_CURRENT_LIST_DIR}/AggroSTL/CMakeLists.txt)

target_compile_features(aggrostl INTERFACE cxx_std_20)
target_compile_options(aggrostl INTERFACE ${flags})
set_target_properties(aggrostl PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(arrays PRIVATE cxx_std_20)
target_compile_options(arrays PRIVATE ${flags})
target_include_directories(arrays PRIVATE ${CMAKE_CURRENT_LIST_DIR}/AggroSTL/aggro/)
target_link_libraries(arrays PRIVATE aggrostl)

target_compile_features(lists PRIVATE cxx_std_20)
target_compile_options(lists PRIVATE ${flags})
target_include_directories(lists PRIVATE ${CMAKE_CURRENT_LIST_DIR}/AggroSTL/aggro/)
target_link_libraries(lists PRIVATE aggrostl)

add_test(NAME arraytest COMMAND arrays)
add_test(NAME listtest COMMAND lists)
