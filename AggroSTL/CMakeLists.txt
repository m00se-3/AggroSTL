target_sources(
    aggrostl
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/modules/Arrays.ixx
)

target_sources(
    arrays
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/tests/arrtest.cpp
)