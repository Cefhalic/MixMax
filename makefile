

default: all
all: _all
build: _all
buildall: _all
_all: mixmaxfli.so mixmax2.exe

.PHONY: clean modelsim

clean:
	rm -f mixmaxfli.so mixmax2.exe modelsim.ini

modelsim.ini:
	vmap -c

modelsim: modelsim.ini
	vsim -modelsimini ./modelsim.ini -do mti.tcl

mixmaxfli.so:
	g++ -std=c++11 -I/DataStore/modeltech/include -fPIC -shared -o mixmaxfli.so mixmaxfli.cpp

mixmax2.exe:
	g++ -std=c++11 -o mixmax2.exe mixmax2.cxx

