#!/bin/bash

umount test
rmmod vfat_log_lecture.ko
rmmod fat.ko
make clean
modprobe -r rxcache
modprobe -r rxdsk
