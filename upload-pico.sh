#!/bin/bash

UF2_PATH=${1:-build/*.uf2}

if [[ $(uname) == "Darwin" ]]; then
  PICO_DIR=${2:-/Volumes/RPI-RP2}
else
  PICO_DIR=${2:-/media/`whoami`/RPI-RP2}
fi

while [ ! -d "$PICO_DIR" ]; do
	echo "Waiting for $PICO_DIR to exist..."
	sleep 1;
done

cp $UF2_PATH "$PICO_DIR"
