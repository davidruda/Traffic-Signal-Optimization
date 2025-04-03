CMAKE_BUILD_DIR=build/cmake

BUILD_TYPE=Release
EXTRA_FLAGS=

VENV=.venv
VENV_ABS_PATH=$(abspath $(VENV))

# https://stackoverflow.com/questions/70675848/makefile-throws-an-error-for-checking-the-os
# https://venthur.de/2021-03-31-python-makefiles.html
# OS agnostic
ifeq ($(OS), Windows_NT)
	PYTHON=python
	VENV_BIN=$(VENV)/Scripts
	COMPILED_OPERATORS=operators.%.pyd
else # Linux
	PYTHON=python3
	VENV_BIN=$(VENV)/bin
	COMPILED_OPERATORS=operators.%.so
endif

all: test_package_cmake test

.PHONY: all clean test test_package_cmake test_package_python test_optimizer

clean:
	rm -rf $(VENV) $(CMAKE_BUILD_DIR) operators*.{c,pyd,so}

$(VENV): requirements-test.txt requirements.txt
	$(PYTHON) -m venv $(VENV)
	$(VENV_BIN)/pip install -r requirements-test.txt
	$(VENV_BIN)/pip install -r requirements.txt

$(CMAKE_BUILD_DIR):
	mkdir -p $(CMAKE_BUILD_DIR)
# Use Python_ROOT_DIR to ensure that Python from the venv is used
	cd $(CMAKE_BUILD_DIR) && \
	cmake ../../traffic_signaling -DBUILD_PYBIND_MODULES=ON -DPython_ROOT_DIR=$(VENV_ABS_PATH) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(EXTRA_FLAGS) && \
	cmake --build . --config $(BUILD_TYPE) -j 4

test_package_cmake: $(VENV) $(CMAKE_BUILD_DIR)
	cd $(CMAKE_BUILD_DIR) && \
	ctest -C $(BUILD_TYPE) -V --output-on-failure

test_package_python: $(VENV)
	$(VENV_BIN)/$(PYTHON) -m unittest discover -s traffic_signaling/tests

$(COMPILED_OPERATORS): operators.py
	$(VENV_BIN)/cythonize -3i operators.py

test_optimizer: $(VENV) $(COMPILED_OPERATORS)
	$(VENV_BIN)/$(PYTHON) -m unittest discover -s tests

test: test_package_python test_optimizer
