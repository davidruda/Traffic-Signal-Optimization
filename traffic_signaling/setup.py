from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = '0.0.1'

ext_modules = [
    Pybind11Extension(
        'traffic_signaling',
        sources=[
            *glob('src/*.cpp'),
            *glob('src/*/*.cpp')
        ],
        include_dirs=['include'],
        cxx_std=20
    ),
]

setup(
    name='traffic_signaling',
    version=__version__,
    author='David Ruda',
    ext_modules=ext_modules,
    cmdclass={'build_ext': build_ext},
    packages=['traffic_signaling'],
    package_data={
        'traffic_signaling': [
            '__init__.pyi',
            'py.typed',
            'city_plan/__init__.pyi',
            'simulation/__init__.pyi'
        ],
    },
    python_requires='>=3.7'
)
