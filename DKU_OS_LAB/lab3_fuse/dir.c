/*
*	Operating System Lab
*	    Lab3 (FUSE file System)
*	    Student id : 
*	    Student name : 
*
*   dir.c :
*       - operations for manipulating directories, including adding entries to directories and 
*         walking the directory structure on-disk to access file.
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
#include <dirent.h>

#include "lab3_fs_types.h"

/*
 * You need to implement directoy related funcions in here.
 */
