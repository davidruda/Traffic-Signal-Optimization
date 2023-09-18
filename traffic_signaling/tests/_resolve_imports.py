# Try to import the modules even if the package is not installed via pip
#
# This is used by CTest when running python tests; if the python package
# is not installed, the modules can still be imported by adding their paths
# to PYTHONPATH

try:
    from traffic_signaling.data import *
except ModuleNotFoundError:
    from data import *

try:
    from traffic_signaling.city_plan import *
except ModuleNotFoundError:
    from city_plan import *

try:
    from traffic_signaling.simulation import *
except ModuleNotFoundError:
    from simulation import *