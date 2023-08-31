
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