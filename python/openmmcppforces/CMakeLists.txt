# Execute SWIG to generate source code for the Python module.

add_custom_target(PythonWrapper DEPENDS "${MODULE_NAME}.i")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}.i ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/py.typed ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

file(GLOB TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests/)
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
    DEPENDS ${MODULE_NAME}.i ${TEST_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
