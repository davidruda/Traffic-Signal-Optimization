PYTHON = python3
PYTHON_WIN = py

INPUT_FOLDER = input_data/
OUTPUT_FOLDER = output/

IN_SUFFIX = .in.txt
OUT_SUFFIX = .out.txt

DATA = \
	a_an_example \
	b_by_the_ocean \
	c_checkmate \
	d_daily_commute \
	e_etoile \
	f_forever_jammed

DATA_IN := $(addprefix $(INPUT_FOLDER), $(addsuffix $(IN_SUFFIX), $(DATA)))
DATA_OUT := $(addprefix $(OUTPUT_FOLDER), $(addsuffix $(OUT_SUFFIX), $(DATA)))

all: $(OUTPUT_FOLDER) $(DATA_OUT)

.PHONY: all all-win python-win clean

all-win: $(OUTPUT_FOLDER) python-win $(DATA_OUT)

$(OUTPUT_FOLDER)%$(OUT_SUFFIX): $(INPUT_FOLDER)%$(IN_SUFFIX)
	$(PYTHON) create_output.py -i $< -o $@

$(OUTPUT_FOLDER):
	mkdir -p $@

python-win:
	$(eval PYTHON = $(PYTHON_WIN))

clean:
	rm -rf $(OUTPUT_FOLDER)
