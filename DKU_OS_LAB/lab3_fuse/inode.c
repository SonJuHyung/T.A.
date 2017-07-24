/*
*	Operating System Lab
*	    Lab3 (FUSE file System)
*	    Student id : 
*	    Student name : 
*
*   inode.c :
*       - inode related operatios.
*
*/
#include <fuse.h>
#include <ulockmgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/file.h> 

#include "lab3_fs_types.h"


/*
 *
 * You need to Implement Inoe related functions in here. 
 */
