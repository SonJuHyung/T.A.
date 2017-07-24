#
#	Operating System Lab
#	    Lab3 (File System in Userspace)
#	    Copyright (C) date/name/email
#	    First Writing: date/month/year
#
#   MaKEFILE :
#       - Makefile for example FUSE code. 

CC = gcc

CFLAGS = -g -W
CFLAGS += -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=30 -I/usr/include/fuse 

LDFLAGS = -lfuse 

OBJS_EXAMPLE = lab3_example.o

SRCS = $(OBJS_EXAMPLE:.o=.c) 

TARGET_EXAMPLE = lab3_example

.SUFFIXES : .c .o

.c.o:
	@echo "Compiling $< ..."
	$(CC) -c -D_GNU_SOURCE $(CFLAGS) -o $@ $<

$(TARGET_EXAMPLE) : $(OBJS_EXAMPLE)
	$(CC) -o $(TARGET_EXAMPLE) $(OBJS_EXAMPLE) $(LDFLAGS) 

all : $(TARGET_EXAMPLE)

clean :
	@echo "Cleaning $< ..."
	rm -rf $(OBJS_EXAMPLE) $(TARGET_EXAMPLE)

new :
	$(MAKE) clean
	$(MAKE)
