

default: all
all: _all
build: _all
buildall: _all
_all: testbench/mixmaxfli.so mixmax2.exe mixmax_orig.exe

.PHONY: clean modelsim

clean:
	rm -rf testbench/mixmaxfli.so mixmax2.exe mixmax_orig.exe modelsim.ini modelsim

modelsim.ini:
	vmap -c

modelsim: modelsim.ini testbench/mixmaxfli.so
	vsim -modelsimini ./modelsim.ini -do testbench/MixMax.tcl

testbench/mixmaxfli.so:
	g++ -std=c++11 -Isoftware -I/DataStore/modeltech/include -fPIC -shared -o testbench/mixmaxfli.so testbench/mixmaxfli.cpp

mixmax2.exe:
	g++ -std=c++11 -o mixmax2.exe software/mixmax2.cxx

mixmax_orig.exe:
	g++ -std=c++11 -o mixmax_orig.exe software/mixmax_orig.cxx
