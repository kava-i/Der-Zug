all:
	cd build && cmake .. && make
run:
	cd src && ../bin/txtadventure
