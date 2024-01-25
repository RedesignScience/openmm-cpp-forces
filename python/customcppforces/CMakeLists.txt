# Execute SWIG to generate source code for the Python module.
add_custom_target(PythonWrapper DEPENDS "${MODULE_NAME}.i")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}.i ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py)
foreach(file ${TEST_FILES})
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/tests COPYONLY)
endforeach(file ${TEST_FILES})

add_custom_command(
    TARGET PythonWrapper
    COMMAND "${SWIG_EXECUTABLE}"
        -python -c++
        -doxygen
        -o "${WRAP_FILE}"
        "-I${OPENMM_DIR}/include"
        "${MODULE_NAME}.i"
    DEPENDS "${MODULE_NAME}.i"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

subdirs (tests)