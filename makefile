whatever=`getconf _NPROCESSORS_ONLN`
ok=$(whatever)

allstar:
	git config core.hooksPath .githooks
	make --no-print-directory txtadventure || make --no-print-directory txtadventure_install

txtadventure_install: install
	cd build && make txtadventure -j$(ok)

txtadventure:
	cd build && make txtadventure -j$(ok)

all: txtadventure server unittesting

server: install
	cd build && make txtadventure_server -j$(ok)
unittesting: install
	cd build && make unittest -j$(ok)
	bin/unittest

run_server:
	cd src && ../bin/txtadventure_server
run:
	cd src && ../bin/txtadventure

install:
	git submodule init
	git submodule update
	cd build && cmake ../src
