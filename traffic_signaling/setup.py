from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

# If the package somehow builds in Debug mode,
# try running `CPPFLAGS="-O3" pip install ./traffic_signaling`

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
        cxx_std=20,
        extra_compile_args=['-O3']
    ),
    Pybind11Extension(
        f'{PACKAGE_NAME}.{SIMULATION_MODULE_NAME}',
        sources=sorted([
            *glob(f'src/{CITY_PLAN_MODULE_NAME}/*.cpp'),
            *glob(f'src/{SIMULATION_MODULE_NAME}/*.cpp'),
            f'src/bindings/{SIMULATION_MODULE_NAME}.cpp'
        ]),
        include_dirs=['include'],
        cxx_std=20,
        extra_compile_args=['-O3']
    )
]

setup(
    version=__version__,
    ext_modules=ext_modules,
    cmdclass={'build_ext': build_ext},
)
