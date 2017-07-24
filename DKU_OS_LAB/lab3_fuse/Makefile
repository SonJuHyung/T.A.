#
#	Operating System Lab
#	    lab3 (File System in Userspace)
#	    Copyright (C) 2017 Juhyung Son <tooson9010@gmail.com>
#	    First Writing: 30/12/2016
#
#   MaKEFILE :
#       - Makefile for Lab3 fuse file system. 
#

CC = gcc
INC = -I${CURDIR}/include/

CFLAGS = -g -W $(INC)
CFLAGS += -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=30 -I/usr/include/fuse 

LDFLAGS = -lfuse 

OBJS_FUSE = fuse_main.o file.o dir.o inode.o mount.o bmap.o
#OBJS_FORMAT = fuse_format_main.o fsformat.o

SRCS = $(OBJS_FUSE:.o=.c) 
#$(OBJS_FORMAT:.o=.c)

TARGET_FUSE = lab3_mount
#TARGET_FORMAT = lab3_mkfs

.SUFFIXES : .c .o

.c.o:
	@echo "Compiling Lab3 $< ..."
	$(CC) -c -D_GNU_SOURCE $(CFLAGS) -o $@ $<

$(TARGET_FUSE) : $(OBJS_FUSE)
	$(CC) -o $(TARGET_FUSE) $(OBJS_FUSE) $(LDFLAGS) 

#$(TARGET_FORMAT) : $(OBJS_FORMAT)
#	$(CC) -o $(TARGET_FORMAT) $(OBJS_FORMAT)

all : $(TARGET_FUSE) 
	#$(TARGET_FORMAT)

dep : 
	gccmaedep $(INC) $(SRCS)

clean :
	@echo "Cleaning Lab3 $< ..."
	rm -rf $(OBJS_FUSE) $(TARGET_FUSE) 
	#$(OBJS_FORMAT) $(TARGET_FORMAT)

new :
	$(MAKE) clean
	$(MAKE)
