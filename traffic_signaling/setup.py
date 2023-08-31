from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = '0.0.1'

PACKAGE_NAME = 'traffic_signaling'
CITY_PLAN_MODULE_NAME = 'city_plan'
SIMULATION_MODULE_NAME = 'simulation'

ext_modules = [
    Pybind11Extension(
        f'{PACKAGE_NAME}.{CITY_PLAN_MODULE_NAME}',
        sources=sorted([
            *glob(f'src/{CITY_PLAN_MODULE_NAME}/*.cpp'),
            f'src/bindings/{CITY_PLAN_MODULE_NAME}.cpp'
        ]),
        include_dirs=['include'],
        cxx_std=20
    ),
    Pybind11Extension(
        f'{PACKAGE_NAME}.{SIMULATION_MODULE_NAME}',
        sources=sorted([
            *glob(f'src/{CITY_PLAN_MODULE_NAME}/*.cpp'),
            *glob(f'src/{SIMULATION_MODULE_NAME}/*.cpp'),
            f'src/bindings/{SIMULATION_MODULE_NAME}.cpp'
        ]),
        include_dirs=['include'],
        cxx_std=20
    )
]

setup(
    name=PACKAGE_NAME,
    version=__version__,
    author='David Ruda',
    ext_modules=ext_modules,
    cmdclass={'build_ext': build_ext},
    packages=[PACKAGE_NAME],
    package_data={
        PACKAGE_NAME: ['*.pyi', 'py.typed', 'data/*.txt']
    },
    python_requires='>=3.7'
)
