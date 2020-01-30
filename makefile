all:
	make --no-print-directory sec
sec:
	(cd build && cmake --build . -j`getconf _NPROCESSORS_ONLN`) || (mkdir -p build && cd build && cmake .. && cmake --build . -j`getconf _NPROCESSORS_ONLN`)
run:
	cd src && ../bin/txtadventure
