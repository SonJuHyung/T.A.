#fat recovery

fat_recovery2.out: fat_recovery.o
	gcc -o fat_recovery fat_recovery.o

fat_recovery.o : fat_info.h fat_recovery.c
	gcc -c fat_recovery.c

clean :
	rm *.o fat_recovery
