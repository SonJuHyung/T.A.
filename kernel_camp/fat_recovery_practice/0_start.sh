#!/bin/bash

mkfs.vfat /dev/rxd0
mount -t vfat_lecture /dev/rxd0 ./disk
touch disk/A && echo "A_FILE_DATA" >> disk/A
touch disk/B && echo "B_FILE_DATA" >> disk/B
touch disk/C && echo "C_FILE_DATA" >> disk/C
sleep 1
umount ./disk
sleep 1
mount -t vfat_lecture /dev/rxd0 ./disk
df -hT
