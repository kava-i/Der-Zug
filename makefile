whatever=`getconf _NPROCESSORS_ONLN`
ok=$(whatever)
server:

txtadventure: install
	cd build && cmake ../src && make txtadventure -j$(ok)
all: txtadventure server unittesting

server: install
	cd build && cmake ../src && make txtadventure_server -j$(ok)
unittesting: install
	cd build && cmake ../src && make unittest -j$(ok)
	bin/unittest

run_server:
	cd src && ../bin/txtadventure_server
run:
	cd src && ../bin/txtadventure

install:
	git submodule init
	git submodule update
