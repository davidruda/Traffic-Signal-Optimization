from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"

ext_modules = [
    Pybind11Extension(
        "simulation",
        sources=sorted(glob("*/*.cpp")),  # Sort source files for reproducibility
        include_dirs=["include"],
    ),
]

setup(
    name="simulation",
    version=__version__,
    author="David Ruda",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    python_requires=">=3.7"
)
