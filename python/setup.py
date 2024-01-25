from setuptools import setup, Extension
import os
import platform

openmm_dir = '@OPENMM_DIR@'
header_dir = '@PLUGIN_HEADER_DIR@'
library_dir = '@PLUGIN_HEADER_DIR@'

# setup extra compile and link arguments on Mac
extra_compile_args = ['-std=c++17']
extra_link_args = []

if platform.system() == 'Darwin':
    extra_compile_args += ['-stdlib=libc++', '-mmacosx-version-min=10.7']
    extra_link_args += [
        '-stdlib=libc++', '-mmacosx-version-min=10.7', '-Wl', '-rpath', openmm_dir+'/lib'
    ]

os.environ['CC'] = '@CMAKE_C_COMPILER@'
os.environ['CXX'] = '@CMAKE_CXX_COMPILER@'

extension = Extension(
    name='_customcppforces',
    sources=['CustomCPPForcesWrapper.cpp'],
    libraries=['OpenMM', 'CustomCPPForces'],
    include_dirs=[os.path.join(openmm_dir, 'include'), header_dir],
    library_dirs=[os.path.join(openmm_dir, 'lib'), library_dir],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

setup(
    name='customcppforces',
    version='@PROJECT_VERSION@',
    py_modules=['customcppforces'],
    ext_modules=[extension],
)
