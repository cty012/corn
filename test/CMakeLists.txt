enable_testing()  # Enables testing functionalities
find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_HEADERS "${CMAKE_SOURCE_DIR}/test/*.h")
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")

add_executable(test ${TEST_HEADERS} ${TEST_SOURCES})
set_target_properties(test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_DIR}")
target_include_directories(test PRIVATE
        "${GTEST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/test/common"
        "${CMAKE_SOURCE_DIR}/include"
)
target_link_libraries(test ${GTEST_BOTH_LIBRARIES} corn)

if (NOT ${CORN_OUTPUT_DIR} STREQUAL ${TEST_OUTPUT_DIR})
    add_custom_target(test_corn ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CORN_OUTPUT_PATH}"
                "${TEST_OUTPUT_DIR}"
            COMMENT "Copied corn DLL to test output directory"
    )
    add_dependencies(test test_corn)
endif ()
