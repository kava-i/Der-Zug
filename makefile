whatever=`getconf _NPROCESSORS_ONLN`
ok=$(whatever)
all:
	make --no-print-directory spec
server:
	make --no-print-directory spec_server
spec: install
	-rm build/CMakeCache.txt
	mkdir -p build && cd build && cmake ../src && make txtadventure -j$(ok)
spec_server: install
	-rm build/CMakeCache.txt
	mkdir -p build && cd build && cmake ../src -DMAKE_SERVER=1 && make txtadventure -j$(ok)
unittest:
	-rm build/CMakeCache.txt
	mkdir -p build && cd build && cmake ../src && make unittest -j$(ok)
	bin/unittest

run:
	cd src && ../bin/txtadventure

install:
	git submodule init
	git submodule update
