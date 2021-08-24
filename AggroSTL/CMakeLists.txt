target_sources(
    aggrostl
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/aggro/array.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/list.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/optional.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/deque.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/concepts/allocator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/concepts/stream.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/allocators/standard.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/allocators/standard.hpp
    ${CMAKE_CURRENT_LIST_DIR}/aggro/profile.hpp
)

target_sources(
    arrays
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/tests/arrtest.cpp
)

target_sources(
    lists
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/tests/listtest.cpp
)