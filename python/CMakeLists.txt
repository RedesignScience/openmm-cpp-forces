set(MODULE_NAME openmmcppforces)
set(WRAP_FILE OpenMMCPPForcesWrapper.cpp)

# Compile the Python module.

add_custom_target(PythonInstall DEPENDS ${SHARED_CUSTOM_CPP_FORCES_TARGET} PythonWrapper)
set(PLUGIN_HEADER_DIR "${CMAKE_SOURCE_DIR}/openmmcppforces/include")
set(PLUGIN_LIBRARY_DIR "${CMAKE_BINARY_DIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.in ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET PythonInstall
    COMMAND "${PYTHON_EXECUTABLE}" -m pip install --no-deps --ignore-installed .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

subdirs(openmmcppforces)
