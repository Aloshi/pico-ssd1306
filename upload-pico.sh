#!/bin/bash

UF2_PATH=${1:-build/*.uf2}
PICO_DIR=${2:-/media/`whoami`/RPI-RP2}

while [ ! -d "$PICO_DIR" ]; do
	echo "Waiting for $PICO_DIR to exist..."
	sleep 1;
done
cp $UF2_PATH "$PICO_DIR"
