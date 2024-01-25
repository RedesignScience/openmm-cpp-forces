add_custom_target(PythonTest DEPENDS "${TEST_FILES}")
add_custom_command(
    TARGET PythonTest
    COMMAND "${PYTHON_EXECUTABLE}" -m pytest --verbose Test*.py
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
