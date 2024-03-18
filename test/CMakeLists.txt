find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TEST_HEADERS "${CMAKE_SOURCE_DIR}/test/*.h")
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")

add_executable(corn_test ${TEST_HEADERS} ${TEST_SOURCES})
set_target_properties(corn_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_DIR}")
target_include_directories(corn_test PRIVATE
        "${CMAKE_SOURCE_DIR}/test/common"
        "${CMAKE_SOURCE_DIR}/include"
)
target_link_libraries(corn_test GTest::gtest GTest::gtest_main corn)

if (NOT ${CORN_OUTPUT_DIR} STREQUAL ${TEST_OUTPUT_DIR})
    add_custom_target(corn_test_dll ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CORN_OUTPUT_PATH}"
                "${TEST_OUTPUT_DIR}"
            COMMENT "Copied corn DLL to test output directory"
    )
    add_dependencies(corn_test corn_test_dll)
endif ()
