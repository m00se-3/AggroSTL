target_sources(
    aggrostl
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/array.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/concepts/allocator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/concepts/stream.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/allocators/standard.hpp
)

target_sources(
    arrays
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/tests/arrtest.cpp
)