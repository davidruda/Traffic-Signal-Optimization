# Try to import the modules even if the package is not installed via pip
#
# This is used by CTest when running python tests; if the python package
# is not installed, the modules can still be imported by adding their paths
# to PYTHONPATH

try:
    from traffic_signaling.data import get_data_filename
except ModuleNotFoundError:
    from data import get_data_filename

try:
    from traffic_signaling.city_plan import CityPlan
except ModuleNotFoundError:
    from city_plan import CityPlan

try:
    from traffic_signaling.simulation import Simulation
except ModuleNotFoundError:
    from simulation import Simulation