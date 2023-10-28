# Intermediate target
add_custom_target(examples
        COMMENT "Built all examples"
)

# Build examples
function (build_example EXAMPLE_NAME)
    if (MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif ()

    file(GLOB EXAMPLE_HEADERS "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.h")
    file(GLOB EXAMPLE_SOURCES "${CMAKE_SOURCE_DIR}/examples/${EXAMPLE_NAME}/*.cpp")
    if (WIN32)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
        # add_executable(${EXAMPLE_NAME} WIN32 ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
    elseif (APPLE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "@executable_path"
        )
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN"
        )
    endif ()
    set_target_properties(${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${EXAMPLE_OUTPUT_DIR}")
    target_include_directories(${EXAMPLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_link_libraries(${EXAMPLE_NAME} corn)
    add_dependencies(${EXAMPLE_NAME} examples)
endfunction ()

build_example("clangy-bird")

# Move corn DLL to example folder
if (NOT ${CORN_OUTPUT_DIR} STREQUAL ${EXAMPLES_OUTPUT_DIR})
    add_custom_command(
            TARGET examples
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CORN_OUTPUT_PATH}
                ${EXAMPLES_OUTPUT_DIR}
            COMMENT "Copied corn DLL to examples output directory"
    )
endif ()
