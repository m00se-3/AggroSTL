target_sources(
    aggrostl
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/Arrays.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/concepts/AllocConcepts.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/concepts/StreamConcepts.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/allocators/STDAllocator.hpp
)

target_sources(
    arrays
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/tests/arrtest.cpp
)