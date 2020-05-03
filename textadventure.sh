#!/bin/bash
cd ~/Documents/programming/Der-Zug/src/
python3 toJson.py
cd ~/Documents/programming/Der-Zug/
make
./startGame.sh
make run

