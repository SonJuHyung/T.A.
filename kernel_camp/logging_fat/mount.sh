#!/bin/bash



modprobe -r rxdsk
modprobe -r rxcache
modprobe rxdsk
modprobe rxcache
rapiddisk --attach 2048
mkfs.vfat /dev/rxd0
make clean
make
insmod fat.ko
insmod vfat_log_lecture.ko
mount -t vfat_log_lecture /dev/rxd0 test
