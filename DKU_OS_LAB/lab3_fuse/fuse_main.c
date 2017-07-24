/*
*	Operating System Lab
*	    Lab3 (FUSE file System)
*	    Student id : 
*	    Student name : 
*
*   fuse_main.c :
*       - check if the device is formated. 
*       - mount device to mount point.
*       - basic fuse operations.
*
*/

#define FUSE_USE_VERSION 30

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

IOM *iom;

void *lab3_init(struct fuse_conn_info *conn)
{    
    // You need to Implement here
}

static int lab3_getattr(const char *path, struct stat *stbuf)
{	
    // You need to Implement here
}

static int lab3_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    // You need to Implement here
}

static int lab3_mkdir(const char *path, mode_t mode)
{
    // You need to Implement here
}

static int lab3_unlink(const char *path)
{
    // You need to Implement here
}

static int lab3_rmdir(const char *path)
{
    // You need to Implement here
}

static int lab3_rename(const char *from, const char *to)
{

    // You need to Implement here   
}

static int lab3_open(const char *path, struct fuse_file_info *fi)
{
    // You need to Implement here   
}

static int lab3_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    // You need to Implement here   
}

static int lab3_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    // You need to Implement here   
}

static int lab3_release(const char *path, struct fuse_file_info *fi)
{
    // You need to Implement here   
}

static int lab3_create (const char *path, mode_t mode, struct fuse_file_info *fi)
{    
    // You need to Implement here   
}

static int lab3_utimens(const char *path, const struct timespec ts[2])
{
    // You need to Implement here   
}

static struct fuse_operations lab3_oper = {
    .init       = lab3_init,        
    .getattr	= lab3_getattr,     
    .readdir	= lab3_readdir,    
    .mkdir		= lab3_mkdir,       
    .unlink		= lab3_unlink,      
    .rmdir		= lab3_rmdir,       
    .rename		= lab3_rename,      
    .open		= lab3_open,        
    .read		= lab3_read,        
    .write		= lab3_write,       
    .release	= lab3_release,     
    .create     = lab3_create,      
    .utimens    = lab3_utimens      
};

int main(int argc, char *argv[])
{  
    int device_selected = 0; 
    int mountpoint_selected = 0;
    char op;
    char *dev_path = NULL, *mnt_path = NULL;
    int format_type = LAB3_FORMAT_MISSING;
    s32 ret;

    int argc_lab4;
    char **argv_lab4;
    char *dev = "/dev/sdc";
    
    if (argc < 3)
    {
        goto INVALID_ARGS;
    }

    if(argv[1] == NULL){
        printf(" input mount point error\n");
        goto INVALID_ARGS;
    }

    if(argv[2] == NULL){
        printf("input device name error\n");
        goto INVALID_ARGS;
    }

    printf("\n device %s is selected \n",dev);
    printf("\n check if it is formatted to lab or fat ...  \n");

    iom = check_formated(argv[argc-2], argv[argc-1]);
    if(iom != NULL){
        printf("\n---------------------    executin mount    --------------------\n");
        printf("  device name = %s \n", iom->dev_path);
        printf("  mount point = %s \n", iom->mnt_path);

        printf("  FUSE_USE_VERSION = %d \n\n", FUSE_USE_VERSION);
        umask(0);

        int tmp_argc = argc-1;
        char **tmp_argv=(char**)malloc(sizeof(char*)*tmp_argc);
        int i;
        for(i=0 ; i< tmp_argc-1; i++){
            tmp_argv[i] = (char*)malloc(strlen(argv[i]));
            strcpy(tmp_argv[i],argv[i]);
        }
        tmp_argv[i] = (char*)malloc(strlen(argv[argc-1]));

        ret = fuse_main(tmp_argc, tmp_argv, &lab3_oper, NULL);
        if(ret < 0){
            printf("\n---------------------     mount failed     --------------------\n");
            goto ERROR;
        }   

        return LAB3_SUCCESS;
    }

    if(iom)
        close_io_manager(iom);
    if(dev_path)
        free(dev_path);

    return LAB3_SUCCESS;

    if(iom != NULL)
        close_io_manager(iom);

ERROR:
    if(dev_path)
        free(dev_path);
    if(mnt_path)
        free(mnt_path);
    if(iom)
        free((s8*)iom);
    
INVALID_ARGS:
    lab3_fuse_mount_usage(argv[0]);
    lab3_fuse_mount_example(argv[0]);

    return LAB3_INVALID_ARGS;
    
}


