all:
	cd build && cmake .. && make
run:
	cd src && valgrind ../bin/txtadventure
