
CC = gcc
INC = -I$(CURDIR)/include/
CFLAGS = -g $(INC)
LDFLAGS = -lpthread

OBJS_SPL = sync_spl.o 
OBJS_SEM = sync_sem.o 
OBJS_MTX = sync_mtx.o 
OBJS_CND = sync_cnd.o

SRCS = $(OBJS_SEM:.o=.c) $(OBJS_SPL:.o=.c) $(OBJS_MTX:.o=.c) $(OBJS_CND:.o=.c) $(OBJS_RWL:.o=.c) 

TARGET_SPL = sync_spl 
TARGET_SEM = sync_sem
TARGET_MTX = sync_mtx
TARGET_CND = sync_cnd

.SUFFIXES : .c .o

.c.o:
	@echo "Compilingi ITRC day03 Labs :  $< ..."
	$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET_SPL) : $(OBJS_SPL)
	@echo "Start building...  Spinlock " 
	$(CC) -o $(TARGET_SPL) $(OBJS_SPL) $(LDFLAGS)
	@echo "Build done."

$(TARGET_SEM) : $(OBJS_SEM)
	@echo "Start building...  Semaphore " 
	$(CC) -o $(TARGET_SEM) $(OBJS_SEM) $(LDFLAGS)
	@echo "Build done."

$(TARGET_MTX) : $(OBJS_MTX)
	@echo "Start building...  Mutex" 
	$(CC) -o $(TARGET_MTX) $(OBJS_MTX) $(LDFLAGS)
	@echo "Build done."

$(TARGET_CND) : $(OBJS_CND)
	@echo "Start building...  Condition Variable " 
	$(CC) -o $(TARGET_CND) $(OBJS_CND) $(LDFLAGS)
	@echo "Build done."


all : $(TARGET_SPL) $(TARGET_SEM) $(TARGET_MTX) $(TARGET_CND) 

dep : 
	gccmaedep $(INC) $(SRCS)

clean :
	@echo "Cleaning SYNC_sync $< ..."
	rm -rf $(OBJS_SPL) $(OBJS_SEM) $(OBJS_MTX) $(OBJS_CND) 
	rm -rf $(TARGET_SPL) $(TARGET_SEM) $(TARGET_MTX) $(TARGET_CND) 

new :
	$(MAKE) clean
	$(MAKE)
