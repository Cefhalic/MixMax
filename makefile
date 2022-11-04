# -----------------------------------------------------------------------------------------------------------------------
# Andrew W. Rose, 2022
# Imperial College London HEP group
# and
# Centre for High-throughput digital electronics and embedded machine learning
# -----------------------------------------------------------------------------------------------------------------------

# Files for executables
EXECUTABLE_SOURCES = $(sort $(wildcard software/*.cxx) )
EXECUTABLES = $(patsubst software/%.cxx,%.exe,${EXECUTABLE_SOURCES})

default: all
all: _all
build: _all
buildall: _all
_all: ${EXECUTABLES}

.PHONY: help clean modelsim vivado

help:
	@echo -e "Makefile for the MixMax project.\nUsage:"
	@echo "  - make                - Build executables"
	@echo "  - make help           - Display this help message"
	@echo "  - make clean          - Tidy all build products"
	@echo "  - make modelsim       - Launch and run the VHDL and C++ FLI validation in Modelsim"
	@echo "  - make vivado         - Launch and run synthesis and implementation of the VHDL in Vivado"

clean:
	rm -rf testbench/mixmaxfli.so ${EXECUTABLES} *.d modelsim.ini modelsim vivado

modelsim.ini:
	vmap -c

modelsim: modelsim.ini testbench/mixmaxfli.so
	vsim -nolog -modelsimini ./modelsim.ini -do testbench/MixMax.tcl

vivado:
	vivado -nolog -nojournal -source xilinx/MixMax.tcl

testbench/mixmaxfli.so:
	g++ -std=c++11 -Isoftware -I/DataStore/modeltech/include -fPIC -shared -o testbench/mixmaxfli.so testbench/mixmaxfli.cpp

-include $(EXECUTABLES:.exe=.d)

${EXECUTABLES}: %.exe: software/%.cxx
	g++ -std=c++11 -O3 -MMD $^ -o $@	
