/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	   	Copyright (C) data/name/email
*
*   lab2_sync_types.h :
*       - lab2 header file
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*/

#ifndef _LAB2_HEADER_H
#define _LAB2_HEADER_H

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define SUCCESS                0
#define ERROR                 -1

/*
 * thread parameter values.
 */

typedef struct thread_arg{
    int num_iterations;
    int is_sync;
}thread_arg;



#endif /* LAB2_HEADER_H*/
