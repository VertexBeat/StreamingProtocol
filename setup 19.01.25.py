import sys
from setuptools import setup, find_packages, Extension
import pybind11

is_linux = sys.platform.startswith('linux')

ext_modules = []

if is_linux:
    ext_modules = [
        Extension(
            'streaming_protocol.core.streaming_protocol',
            sources=['src/PythonBindings.cpp'],
            include_dirs=[
                'include',
                pybind11.get_include(),
                pybind11.get_include(user=True)
            ],
            extra_compile_args=['-std=c++17'],
        )
    ]


setup(
    name='streaming_protocol',
    version='0.1.0',
    ext_modules=ext_modules,
    packages=find_packages(where='src'),
    package_dir={'': 'src'},  # Tell setuptools to look in src directory
    package_data={
        'streaming_protocol': ['py.typed'],
        'streaming_protocol.core': ['*.pyi']  # Include type stubs
    },
)