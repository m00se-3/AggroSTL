target_sources(
    aggrostl
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/Arrays.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Output.hpp
)

target_sources(
    arrays
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/tests/arrtest.cpp
)