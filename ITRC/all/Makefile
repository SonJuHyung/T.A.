
CC = gcc
# debug level
DEBUG = -g3
#optimization level
OPTIMIZATION = -O0 
# warnning message
#WARNING=
WARNING=-w
#WARNING = -Wall

# headers
INC = $(CURDIR)/include/
CFLAGS = $(DEBUG) $(WARNING) -pthread -I$(INC) -D_GNU_SOURCE
LDFLAGS = -lpthread -lrt

OBJS_ITRC_SYNC = sync_main.o sync_funcs.o 
OBJS_ITRC_IPC = ipc_main.o ipc_funcs.o 

SRCS_ITRC = $(OBJS_ITRC_SYNC:.o=.c) $(OBJS_ITRC_IPC:.o=.c)

TARGET_ITRC_SYNC = itrc_sync
TARGET_ITRC_IPC = itrc_ipc

#.SUFFIXES : .c .o

.c.o:
	@echo "Compilingi ITRC day03 projects :  $< ..."
	@$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

$(TARGET_ITRC_SYNC) : $(OBJS_ITRC_SYNC)
	@echo "Start building... ITRC Synchronization lab" 
	$(CC) -o $(TARGET_ITRC_SYNC) $(OBJS_ITRC_SYNC) $(LDFLAGS)
	@echo "Build done."


$(TARGET_ITRC_IPC) : $(OBJS_ITRC_IPC)
	@echo "Start building... ITRC Communication lab" 
	$(CC) -o $(TARGET_ITRC_IPC) $(OBJS_ITRC_IPC) $(LDFLAGS)
	@echo "Build done."

all : $(TARGET_ITRC_SYNC) $(TARGET_ITRC_IPC)

dep : 
	gccmaedep $(INC) $(SRCS_ITRC)

clean :
	@echo "Cleaning ITRC Synchronization, Communication Lab, : $< ..."
	rm -rf $(OBJS_ITRC_SYNC) $(TARGET_ITRC_SYNC)
	rm -rf $(OBJS_ITRC_IPC) $(TARGET_ITRC_IPC)
	@echo "Cleaned. $< ..."

