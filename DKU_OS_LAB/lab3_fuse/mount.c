/*
*	Operating System Lab
*	    Lab3 (FUSE file System)
*
*   mount.c :
*       - the main source to make lab3_mount executable file.
*
*/


#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/file.h>

#include "lab3_fs_types.h"

void lab3_fuse_mount_usage(char *cmd)
{
    printf("DKU Operating System lab3 - fuse file system \n");
	printf("\n Usage for %s : \n",cmd);
	printf("    -argv[1] : device name to mount (e.g. /dev/sdc /dev/name0n1 )\n");
	printf("    -argv[2] : mount point (e.g. /mnt /home/son )\n\n");
}

void lab3_fuse_mount_example(char *cmd)
{
    printf("\n Example : \n");
    printf("    #sudo %s /dev/sdc /mnt \n", cmd);
    printf("    #sudo %s /dev/nvme0n1 /home/son \n\n", cmd);
}


#define lab3_FORMAT_UNKNOWN LAB3_FORMAT_MISSING

IOM* check_formated(s8 *dev, s8* mnt){

    IOM *iom = NULL;
    struct lab3_super_block *sb;
    s8* buf_path, *buf_sb;
    s8 *buf_ibmap, *buf_dbmap, *buf_itble;
    s32 fid, cluster_size;

    s8 *dev_path, *mnt_path;

    iom = (IOM*)malloc(sizeof(IOM));
    if(!iom)
        return NULL;
    memset(iom, 0x0,sizeof(IOM));

    dev_path = (char*)malloc(strlen(dev)+1);
    memset(dev_path, 0x0, strlen(dev)+1);
    sprintf(dev_path, "%s", dev);
    strcpy(dev_path, dev); 

    mnt_path = (char*)malloc(strlen(mnt)+1);
    memset(mnt_path, 0x0, strlen(mnt)+1);
    strcpy(mnt_path, mnt);

    fid = open(dev_path, O_RDWR);
    if(fid == LAB3_ERROR){
        printf("can't open device %s \n",dev_path);
        return NULL;
    }

    iom->dev_fd = fid;
    iom->dev_path = dev_path;
    iom->mnt_path = mnt_path;

    fill_io_manager(iom, lab3_FORMAT_UNKNOWN);

    if(iom){
        cluster_size = iom->cluster_size;
        buf_sb = (s8*)malloc(cluster_size);
        if(!buf_sb){
            printf("    malloc error \n");
            goto ERROR;
        }

        lseek(fid,RPOS_SB * cluster_size, SEEK_SET);
        printf("\n---------------------  format inspection   --------------------\n");

        printf("read super block for checking ... \n");
        if(read(fid, buf_sb, cluster_size) == LAB3_ERROR)
            goto ERROR;

        printf("inspecting if signature value is match ...\n");

        sb = (struct lab3_super_block*)buf_sb;
        iom->sb = sb;
                
        printf("    signature inspection is computed. \n\n");
        if(sb->sb_signature == LAB3_SB_SIGNATURE){

            printf("     - file system signature :   %x \n", sb->sb_signature);
            printf("     - OS Lab student id     :   %x \n\n", sb->sb_student_id);
            printf("    current %s is formated to lab \n", iom->dev_path);
            printf("    you can mount this device to mount point  \n");

            buf_ibmap = (s8*)malloc(cluster_size);
            lseek(fid,RPOS_IBMAP * cluster_size, SEEK_SET);
            read(fid, buf_ibmap, cluster_size);
            iom->iom_buf_ibmap = buf_ibmap;

            buf_dbmap = (s8*)malloc(cluster_size);
            lseek(fid,RPOS_DBMAP * cluster_size, SEEK_SET);
            read(fid, buf_dbmap, cluster_size);
            iom->iom_buf_dbmap = buf_dbmap;

            buf_itble = (s8*)malloc(cluster_size);
            lseek(fid,RPOS_ITBLE * cluster_size, SEEK_SET);
            read(fid, buf_itble, cluster_size);
            iom->iom_buf_itble = buf_itble;

        }else{
            printf("    signature missmatch. current signature value is %x  \n", sb->sb_signature);
            printf("    you can't mount this device to mount point  \n");

            goto ERROR;
        }
    }else{
        printf("    setting io manager error \n");
        return 0;
    }
	printf("\n--------------------- inspection successed --------------------\n");

    return iom;   
ERROR:
    printf("    mount failed  \n");
	printf("\n---------------------  inspection failed   --------------------\n");

    if(buf_sb)
        free(buf_sb);

    return NULL;
}

u64 get_no_of_sectors(IOM *iom)
{
	u64 blk_size=0,no_of_sectors=0;

    if(!iom)
        return LAB3_INVALID_ARGS;
    

	ioctl(iom->dev_fd, BLKGETSIZE, &no_of_sectors);
    ioctl(iom->dev_fd, BLKSSZGET, &blk_size);

    iom->no_of_sectors = no_of_sectors;
    iom->blk_size = blk_size;
    iom->cluster_size = iom->blk_size << LAB3_BLK_SIZE_SHIFT;

	return 0;
}


s32 check_flag(s32 fid,s32 *state_flag, s32 *access_flag){

    s32 cur_flag;

    cur_flag = fcntl(fid, F_GETFL,0);
    if(cur_flag == LAB3_ERROR)
        return LAB3_ERROR;

    *state_flag = cur_flag; 
    *access_flag = *state_flag & O_ACCMODE;

    return LAB3_SUCCESS;
}


s32 fill_io_manager(IOM *iom,int format_type){

    s32 access_flag, state_flag;

    if(format_type != LAB3_FORMAT_MISSING)        
        iom->format_type = format_type;
    else
        iom->format_type = LAB3_FORMAT_MISSING;

    if(check_flag(iom->dev_fd, &state_flag, &access_flag) == LAB3_ERROR)
        return LAB3_ERROR;

    if(access_flag != O_WRONLY){
        if(get_no_of_sectors(iom) != LAB3_SUCCESS){
            printf("    error in fill_io_manager \n");
            return LAB3_ERROR;
        }
    }else{
        printf("    errpr in fill_io_manager - can't read %s device... \n \
                        (current flag is O_WRONLY)",iom->dev_path);
    }
    


    return LAB3_SUCCESS;
   
}

s32 close_io_manager(IOM *iom){
    if(iom){
        if(iom->dev_path){
            close(iom->dev_fd);
            free(iom->dev_path);
        }
        free(iom);
    }
    return LAB3_SUCCESS;
}



