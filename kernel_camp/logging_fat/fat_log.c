/*
 *  Linux Kernel Camp 2016-08-25
 *
 *  logging functions
 *
 */


#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/pagemap.h>
#include <linux/mpage.h>
#include <linux/buffer_head.h>
#include <linux/mount.h>
#include <linux/aio.h>
#include <linux/vfs.h>
#include <linux/parser.h>
#include <linux/uio.h>
#include <linux/writeback.h>
#include <linux/log2.h>
#include <linux/hash.h>
#include <linux/blkdev.h>
#include <linux/kernel.h> 
#include <asm/unaligned.h>
#include "fat.h"


/*
 * Linux Kernel Camp 2016-08-25
 *
 * get msdos_dir_entry through blocknr where the file exist and the offset of the file in the directory 
 * 
 */
struct msdos_dir_entry* get_dir_entry_from_blocknr(struct super_block*sb, sector_t blocknr, int offset, struct buffer_head **bh_temp)
{
	struct msdos_dir_entry *raw_entry = NULL;
	struct buffer_head *bh = NULL;
	bh = sb_bread(sb, blocknr);
	if (!bh) {
		printk("<0>FAT_SON  get_dir_entry_from_blocknr - sb_bread 오류 \n");       
		return NULL;
	}

	*bh_temp = bh;    
	raw_entry = &((struct msdos_dir_entry *) (bh->b_data))[offset];
	return raw_entry;
}


/*
 * Linux Kernel Camp 2016-08-25
 *
 * Log read or write event in log area
 *
 */
int log_read_write(struct super_block *sb,struct inode *inode )
{
	struct msdos_sb_info *sbi = MSDOS_SB(sb);
	struct buffer_head *bh_log=NULL, *bh_de=NULL;
	struct fat_log_entry *log;
	struct msdos_dir_entry *de;
	sector_t blocknr, log_sectornr;
	int offset;
	loff_t i_pos;

	if(sbi->log_pos > sbi->log_max_pos)
	{
		printk("log_read_write : log is full it needs to be flushed \n");
		return 0;
	}

	log = kmalloc(sizeof(struct fat_log_entry), GFP_NOFS);
	if(!log){
		printk("log_read_write : unable to alloc log \n");
		return 0; 
	}

	i_pos = fat_i_pos_read(sbi, inode);
	if (!i_pos){
		printk("log_read_write : unable to get i_pos \n");
		return 0;
	}

	fat_get_blknr_offset(sbi, i_pos, &blocknr, &offset);

	de = get_dir_entry_from_blocknr(sb,blocknr,offset, &bh_de);
	if(de == NULL)
		goto free;
	memset(log->name, 0, sizeof(unsigned char)*MSDOS_NAME);
	memcpy(log->name, de->name, sizeof(unsigned char)*MSDOS_NAME);

	log->date = de->adate;
	log->time = de->time;

	brelse(bh_de);

	log_sectornr = sbi->log_pos / (sbi->sector_size); 
	offset = (sbi->log_pos) & (sb->s_blocksize - 1);

	bh_log = sb_bread(sb,log_sectornr);
	if(!bh_log)
		goto free_log_none;
	memcpy(bh_log->b_data+offset, log, sizeof(struct fat_log_entry));
		goto out;

free :
	kfree(log);
free_log_none:
	kfree(log);
	brelse(bh_log);
out:
	kfree(log);
	brelse(bh_log);
	sbi->log_pos += sizeof(struct fat_log_entry);
	return 1;
}


