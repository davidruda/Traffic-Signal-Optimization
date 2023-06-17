from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"

ext_modules = [
    Pybind11Extension(
        "traffic_signaling",
        # Sort source files for reproducibility
        sources=sorted([
            *glob("bindings/*.cpp"),
            *glob("src/*.cpp")
        ]),
        include_dirs=["include"],
        extra_compile_args=["-std=c++20"]
    ),
]

setup(
    name="traffic_signaling",
    version=__version__,
    author="David Ruda",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    python_requires=">=3.7"
)
