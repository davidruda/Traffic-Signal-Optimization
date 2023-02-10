PYTHON=python3
PYTHON_WIN=py

INPUT_FOLDER=input_data/
OUTPUT_FOLDER=output/

IN_SUFFIX=.in.txt
OUT_SUFFIX=.out.txt

A_DATA=a_an_example
B_DATA=b_by_the_ocean
C_DATA=c_checkmate
D_DATA=d_daily_commute
E_DATA=e_etoile
F_DATA=f_forever_jammed

A_IN=$(INPUT_FOLDER)$(A_DATA)$(IN_SUFFIX)
A_OUT=$(OUTPUT_FOLDER)$(A_DATA)$(OUT_SUFFIX)

B_IN=$(INPUT_FOLDER)$(B_DATA)$(IN_SUFFIX)
B_OUT=$(OUTPUT_FOLDER)$(B_DATA)$(OUT_SUFFIX)

C_IN=$(INPUT_FOLDER)$(C_DATA)$(IN_SUFFIX)
C_OUT=$(OUTPUT_FOLDER)$(C_DATA)$(OUT_SUFFIX)

D_IN=$(INPUT_FOLDER)$(D_DATA)$(IN_SUFFIX)
D_OUT=$(OUTPUT_FOLDER)$(D_DATA)$(OUT_SUFFIX)

E_IN=$(INPUT_FOLDER)$(E_DATA)$(IN_SUFFIX)
E_OUT=$(OUTPUT_FOLDER)$(E_DATA)$(OUT_SUFFIX)

F_IN=$(INPUT_FOLDER)$(F_DATA)$(IN_SUFFIX)
F_OUT=$(OUTPUT_FOLDER)$(F_DATA)$(OUT_SUFFIX)

output-folder:
	mkdir -p $(OUTPUT_FOLDER)

a: $(A_IN)
	$(PYTHON) create_output.py < $< > $(A_OUT)

b: $(B_IN)
	$(PYTHON) create_output.py < $< > $(B_OUT)

c: $(C_IN)
	$(PYTHON) create_output.py < $< > $(C_OUT)

d: $(D_IN)
	$(PYTHON) create_output.py < $< > $(D_OUT)

e: $(E_IN)
	$(PYTHON) create_output.py < $< > $(E_OUT)

f: $(F_IN)
	$(PYTHON) create_output.py < $< > $(F_OUT)

all: output-folder a b c d e f

a-win: $(A_IN)
	$(PYTHON_WIN) create_output.py < $< > $(A_OUT)

b-win: $(B_IN)
	$(PYTHON_WIN) create_output.py < $< > $(B_OUT)

c-win: $(C_IN)
	$(PYTHON_WIN) create_output.py < $< > $(C_OUT)

d-win: $(D_IN)
	$(PYTHON_WIN) create_output.py < $< > $(D_OUT)

e-win: $(E_IN)
	$(PYTHON_WIN) create_output.py < $< > $(E_OUT)

f-win: $(F_IN)
	$(PYTHON_WIN) create_output.py < $< > $(F_OUT)

all-win: output-folder a-win b-win c-win d-win e-win f-win