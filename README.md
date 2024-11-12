# Traffic signaling

## *Get the latest thesis pdf artifact [HERE](https://gitlab.mff.cuni.cz/teaching/nprg045/fink/david_ruda/-/artifacts)*

## Links

### [Problem description](https://storage.googleapis.com/coding-competitions.appspot.com/HC/2021/hashcode_2021_online_qualification_round.pdf)

[Problem submission and interactive page (Wayback Machine)](https://web.archive.org/web/20230310031948/https://codingcompetitions.withgoogle.com/hashcode/round/00000000004361e2/0000000000c617e4)  
[Google Hash Code archive (Wayback Machine)](https://web.archive.org/web/20230602012017/https://codingcompetitions.withgoogle.com/hashcode/archive)  
<!--
[Kaggle competition with additional data](https://www.kaggle.com/competitions/hashcode-2021-oqr-extension/
overview)
-->
## Easy setup
  - ### Run `make test` to set up the environment and run all tests

### How to build and install the `traffic_signaling` package for use in Python

- C++20 code - `gcc 11`, `clang 16` or `MSVC 19.29` (or later) is required to build the package

- Note that you need to have Python headers installed on your system in order to build the package (some Linux distributions don’t install them by default)

  - You can check the location of the headers by running `python3 -c "import sysconfig; print(sysconfig.get_path('include'))"`
  - If not available, install them; e.g. `sudo apt install python3-dev` on Ubuntu

- Run `pip install ./traffic_signaling` (preferably in a virtual environment)
  - If the package somehow builds in Debug mode, try running `CPPFLAGS="-O3" pip install ./traffic_signaling`

- Optionally, verify the package by running tests:

  - `pip install -r requirements-test.txt`
  - `python3 -m unittest discover -s traffic_signaling/tests`


## Solutions
### [Solution Visualizer](https://yoann-dufresne.github.io/Hashcode_2021_visu/)
- `7,885,740` - 1 sec green for every incoming street
- `8,943,886` - 1 sec green for every used (with at least one car going through) incoming street
