# Intermediate target
add_custom_target(examples
        COMMENT "Built all examples"
)

# Build examples
function (build_example EXAMPLE_NAME)
    file(GLOB EXAMPLE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}/*.h")
    file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}/*.cpp")

    add_executable(${EXAMPLE_NAME} ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES})

    if (WIN32)
        if (MSVC)
            set_target_properties(${EXAMPLE_NAME} PROPERTIES
                    LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:CONSOLE"  # /SUBSYSTEM:WINDOWS
            )
        endif ()
    elseif (APPLE)
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "@executable_path"
        )
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN"
        )
    endif ()

    set_target_properties(${EXAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${EXAMPLES_OUTPUT_DIR}/${EXAMPLE_NAME}")
    target_include_directories(${EXAMPLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_link_libraries(${EXAMPLE_NAME} corn)
    add_dependencies(${EXAMPLE_NAME} examples)

    # Copy corn DLL
    add_custom_target(${EXAMPLE_NAME}_dll ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Copying DLL..."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CORN_OUTPUT_PATH}"
                "${EXAMPLES_OUTPUT_DIR}/${EXAMPLE_NAME}"
    )
    add_dependencies(${EXAMPLE_NAME} ${EXAMPLE_NAME}_dll)

    # Copy resources folder
    file(GLOB_RECURSE RESOURCES "${EXAMPLES_OUTPUT_DIR}/${EXAMPLE_NAME}/resources/*")
    add_custom_target(${EXAMPLE_NAME}_resources ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Copying resources..."
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${EXAMPLES_OUTPUT_DIR}/${EXAMPLE_NAME}"
            COMMAND ${CMAKE_COMMAND} -E remove_directory
                "${EXAMPLES_OUTPUT_DIR}/${EXAMPLE_NAME}/resources"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}/resources"
                "${EXAMPLES_OUTPUT_DIR}/${EXAMPLE_NAME}/resources"
            SOURCES ${RESOURCES}
    )
    add_dependencies(${EXAMPLE_NAME} ${EXAMPLE_NAME}_resources)
endfunction ()

build_example("clangy-bird")
