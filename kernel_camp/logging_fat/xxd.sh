#!/bin/bash

echo fat1 area - 0x4000
xxd -g 4 -l 128 -s+0x4000 /dev/rxd0

echo fat2 area - 0x203000
xxd -g 4 -l 128 -s+0x203000 /dev/rxd0

echo log area - 0x402000
xxd -g 4 -l 128 -s+0x402000 /dev/rxd0

echo data area - 0x601000
xxd -g 4 -l 128 -s+0x601000 /dev/rxd0
