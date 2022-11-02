
default: all
all: _all
build: _all
buildall: _all
_all: testbench/mixmaxfli.so mixmax2.exe mixmax_orig.exe mixmax_orig_clean.exe mixmax.exe

.PHONY: clean modelsim vivado

clean:
	rm -rf testbench/mixmaxfli.so mixmax2.exe mixmax_orig.exe mixmax_orig_clean.exe mixmax.exe modelsim.ini modelsim vivado

modelsim.ini:
	vmap -c

modelsim: modelsim.ini testbench/mixmaxfli.so
	vsim -nolog -modelsimini ./modelsim.ini -do testbench/MixMax.tcl

vivado:
	vivado -nolog -nojournal -source xilinx/MixMax.tcl

testbench/mixmaxfli.so:
	g++ -std=c++11 -Isoftware -I/DataStore/modeltech/include -fPIC -shared -o testbench/mixmaxfli.so testbench/mixmaxfli.cpp

mixmax.exe:
	g++ -std=c++11 -o mixmax.exe software/mixmax.cxx

mixmax2.exe:
	g++ -std=c++11 -o mixmax2.exe software/mixmax2.cxx

mixmax_orig.exe:
	g++ -std=c++11 -o mixmax_orig.exe software/mixmax_orig.cxx

mixmax_orig_clean.exe:
	g++ -std=c++11 -o mixmax_orig_clean.exe software/mixmax_orig_clean.cxx	
