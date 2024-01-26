# Execute SWIG to generate source code for the Python module.

file(GLOB SWIG_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.i)
add_custom_target(PythonWrapper DEPENDS ${SWIG_SOURCE_FILES})
foreach(file ${SWIG_SOURCE_FILES})
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR})
endforeach(file ${SWIG_SOURCE_FILES})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py ${CMAKE_CURRENT_BINARY_DIR})

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
