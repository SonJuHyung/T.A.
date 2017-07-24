
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <fuse.h>

#ifndef _LAB3_HEADER_H
#define _LAB3_HEADER_H

/* ERROR STATUS */
#define	LAB3_ERROR		        -1
#define	LAB3_SUCCESS		     0
#define	LAB3_FORMATERR	         1
#define LAB3_OUT_OF_MEMORY      -1
#define LAB3_DO_NOT_EXIST       -2
#define LAB3_INVALID_ARGS       -2
#define LAB3_FORMAT_MISSING      0
#define FALSE	                 0   
#define TRUE	                 1


/* GENERAL FLAG  */
typedef signed char			s8;	
typedef signed short		s16;
typedef signed int			s32; 
typedef signed long         s64; 
typedef unsigned char		u8;	
typedef unsigned short		u16;
typedef unsigned int		u32; 
typedef unsigned long       u64; 		
typedef u32 inode_t;//inode number
typedef u32 pbno_t;//physical block number
typedef u32 lbno_t;//logical block num
typedef s64 lab3_off_t;
typedef u32 lab3_loff_t;

/*format related  */
#define RPOS_SB     0
#define RPOS_IBMAP  1
#define RPOS_DBMAP  2
#define RPOS_ITBLE  3
#define RPOS_DAREA  4
#define RPOS_RESV   5

struct reserved_pos{
    s8 *buf;
    int pos;
};


/* DATA TYPE FLAG  */
#define DATA 0
#define META 1
#define LAB3_TYPE_DATA           0
#define LAB3_TYPE_META           1
#define LAB3_SUCCESS             0
#define LAB3_FORMAT_LAB          1
#define LAB3_FORMAT_FAT          2


/* CAPACITY CALCULATION */
#	define CLUSTER_SIZE (4*1024)
#	define CLUSTER_SIZE_BITS 12
#   define BITS_PER_CLUSTER 8
#   define BITS_PER_CLUSTER_BITS 3


/* LAB3 SPECIAL SIGNATURE */
#define LAB3_SB_SIGNATURE	    0x3442414C
#define LAB3_SB_STD_ID	        0x60181132

/* INITIAL BLOCK NUMBER */
#define INIT_LAB3_SUPERBLOCK_NO		0

/* INODE NUMBER */
#define LAB3_RESRV0_INO		0
#define LAB3_RESRV1_INO		1
#define LAB3_ROOT_INO		2 /* root directory */
#define LAB3_NUM_RESV_INO   3

#define LAB3_TYPE_UNKOWN	1
#define LAB3_TYPE_INODE		3
#define LAB3_TYPE_FILE		4
#define LAB3_TYPE_INDIRECT	5
#define LAB3_TYPE_DIRECTORY	6
/* DIR RELATED */

#define LAB3_SB_SIZE    sizeof(struct lab3_super_block)
#define LAB3_DIR_ENTRY_SIZE sizeof(struct lab3_dir_entry)
#define LAB3_DIR_ENTRY_NUM (CLUSTER_SIZE/LAB3_DIR_ENTRY_SIZE)
#define FNAME_SIZE (116)
#define LAB3_DIR_EMPTY	(0)
#define LAB3_DIR_USED	(1 << 1)
#define LAB3_DIR_DELETED (1 << 2)
#define LAB3_INODE_ENTRY_SIZE sizeof(struct lab3_inode)
#define LAB3_INODE_ENTRY_NUM	(CLUSTER_SIZE/lab3_INODE_ENTRY_SIZE)

/* # OF MAX OPEN FILE */
#define START_OPEN_FILE	3 /* STDIN, STDOUT, STDERR*/
#define MAX_OPEN_FILE	16
#define LAB3_BLK_SIZE_SHIFT 3

/*
 * superblock structure
 */
struct lab3_super_block{
	u32 sb_signature;   /* file system signature*/
    u32 sb_student_id;  /* your student id  */

	s64	sb_sector_size; /* sector size */
    s64 sb_block_size_shift; /* block size shift bit */
	s64	sb_no_of_sectors;    /* number of sectors in ramdirk  */	
	s64	sb_no_of_used_blocks; /* used block count  */

	u32	sb_root_ino; /* reserved inode count  */
    u32 sb_reserved_ino; /* inode table max inode count  */

	s32	sb_max_dir_num; /* msx directoy in block      */
	s32	sb_max_inode_num; /* max inode count in block */

    u32 sb_reserved;    /* reserved area count in cluster size */
    u32 sb_sb_start;    /* super block start address in cluster size */
    u32 sb_sb_size;     /* super block area size in cluster size */
	u32 sb_ibitmap_start; /* inode bitmap start address in cluster size */
	u32 sb_ibitmap_size; /* inode bitmap size in blocknr in cluster size */
	u32 sb_dbitmap_start; /* data bitmap start address in cluster size */
	u32 sb_dbitmap_size; /* data bitmap area size in cluster size */
	u32 sb_itable_start; /* inode table start address in cluster size*/
	u32 sb_itable_size; /* inode table size in cluster size */
	u32 sb_darea_start; /* data area start address in cluster size*/
	u32 sb_darea_size; /* data area size in cluster size */

	s32 sb_free_inodes; /* free inode count  */
	s64 sb_free_blocks; /* free block count  */

    u32 sb_last_allocated_ino;/* last allocated inode number  */
    u32 sb_last_allocated_blknr; /* last allocated blknr  */
};

/*
 * Directory entry structure
 */
struct lab3_dir_entry{	
	inode_t	d_ino; /* inode number  */
	u32	d_flag; /*directory flag (e.g. LAB2_DIR_USDED , LAB2_DIR_DELETED ..)*/
	s8	d_filename[FNAME_SIZE];	/* file nanme  */
	u32 resv; /* null padding to make dir_entry to 128 byte size  */
};

#define LAB3_SUPERBLOCK_OFFSET  0
#define LAB3_SUPERBLOCK_SIZE    1
#define LAB3_IBITMAP_OFFSET     1
#define LAB3_IBITMAP_SIZE       1
#define LAB3_DBITMAP_OFFSET     (LAB3_IBITMAP_OFFSET+LAB3_IBITMAP_SIZE) 
#define LAB3_DBITMAP_SIZE       1
#define LAB3_ITABLE_OFFSET      (LAB3_DBITMAP_OFFSET+LAB3_DBITMAP_SIZE)
#define LAB3_ITABLE_SIZE        1
#define LAB3_DAREA_OFFSET       (LAB3_ITABLE_OFFSET + LAB3_ITABLE_SIZE) 
#define LAB3_RESERVED           LAB3_DAREA_OFFSET 

#define DIRECT_BLOCKS       11
#define INDIRECT_BLOCKS     12
#define DINDIRECT_BLOCKS    13
#define TINDIRECT_BLOCKS    14
#define INDIRECT_BLOCKS_LEVEL 4

struct lab3_inode{	
	inode_t	i_ino;      /* inode number  */                                
	u32	i_type;         /* file type  */
	s64	i_size;	        /* file size  */                                
	u32	i_deleted;      /* is fil edeleted ?  */                                
	u32	i_links_count;	/* Links count */               
	u32	i_ptr;		    /* for directory entry */       
	u32	i_atime;	    /* Access time */               
	u32	i_ctime;	    /* Inode change time */         
	u32	i_mtime;	    /* Modification time */         
	u32	i_dtime;	    /* Deletion Time */             
	u16	i_gid;		    /* Low 16 bits of Group Id */   
	u16	i_uid;		    /* Low 16 bits of Owner Uid */	
	u16	i_mode;		    /* File mode */                 
	u32 resv1[1];       /* null paddings to make inode size to 128 byte size */
	u32 i_blocks[TINDIRECT_BLOCKS + 1];                 
	u32 resv2[2];	    /* null padding to make inode size to 128 byte size */                                
};


/*
 * LAB3 file system usage context
 */
#define CURRENT_FORMAT(TYPE) \
    (TYPE == LAB3_FORMAT_LAB) ? printf("lab") : printf("fat") 

#define IOM struct io_manager
#define IOM_SB(IOM) {IOM->sb}
#define IOM_SB_I(IOM) {IOM->sbi}
#define SIZE(TYPE) sizeof(struct TYPE)
#define INODE struct lab3_inode
#define DIRENTRY struct lab3_dir_entry

/*
 * io manager structure to read and write to ramdisk
 */
struct io_manager{
    s8 *dev_path; /* device file path (e.g. /dev/rd0 )  */
    s8* mnt_path; /* mount path (e.g. mnt ) */
    int dev_fd; /* opened device file's file descriptor(used to write&read device file)  */
    int no_of_sectors; /* number of sectors in device file  */
    int blk_size; /* block size in device file  */
    int cluster_size; /* cluster size in device file (write, read unit)  */
    int format_type; /* format type (current file system format type)  */
    struct lab3_super_block *sb; /* super block which you read from debvice file  */
    struct lab3_sb_info *sbi; /* super block in  */
    char *iom_buf_ibmap; /* inode bitmap buffer  */
    char *iom_buf_dbmap; /* data bitmap buffer  */
    char *iom_buf_itble; /* inode table bitmap buffer  */
};

/* mount.c      related functions  */
void lab3_fuse_mount_usage(char *cmd);
void lab3_fuse_mount_example(char *cmd);
IOM* check_formated(s8 *dev, s8* mnt);
s32 close_io_manager(IOM *iom);
s32 check_flag(s32 fid,s32 *state_flag, s32 *access_flag);

/* fsformat.c */
s32 format_device(struct io_manager *iom);
s32 free_buf(s8 *buf);
void lab3_fuse_mkfs_usage(char *cmd);
void lab3_fuse_mkfs_example(char *cmd);
IOM* set_io_manager(char *path, s32 flags);
s32 fill_io_manager(IOM *iom,int format_type);
s32 fill_io_manager(IOM *iom,int format_type);

/* inode.c      related functions  */

/* dir.c        related functions  */

/* file.c       related functions  */

/* bmap.c       related functions  */




#endif /* LAB3_HEADER_H*/


