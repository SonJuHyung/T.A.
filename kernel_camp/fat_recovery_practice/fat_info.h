#include <linux/types.h>

#define SECTOR_SIZE 512
#define CLUS_START_NUM 2
#define MSDOS_NAME 11
#define DELETED_FLAG 0xe5    /* marks file as deleted when in name[0] */
#define EOF_FAT32 0x0FFFFFFF
/* This structure is fat boot sector information */
struct fat_boot_sector {
    __u8    ignored[3]; /* Boot strap short or near jump */
    __u8    system_id[8];   /* Name - can be used to special case
                               113                    partition manager volumes */
    __u8    sector_size[2]; /* bytes per logical sector */
    __u8    sec_per_clus;   /* sectors/cluster */
    __le16  reserved;   /* reserved sectors */
    __u8    fats;       /* number of FATs */
    __u8    dir_entries[2]; /* root directory entries */
    __u8    sectors[2]; /* number of sectors */
    __u8    media;      /* media code */
    __le16  fat_length; /* sectors/FAT */
    __le16  secs_track; /* sectors per track */
    __le16  heads;      /* number of heads */
    __le32  hidden;     /* hidden sectors (unused) */
    __le32  total_sect; /* number of sectors (if sectors == 0) */

    union {
        struct {
            /*  Extended BPB Fields for FAT16 */
            __u8    drive_number;   /* Physical drive number */
            __u8    state;      /* undocumented, but used
                                   132                            for mount state. */
            __u8    signature;  /* extended boot signature */
            __u8    vol_id[4];  /* volume ID */
            __u8    vol_label[11];  /* volume label */
            __u8    fs_type[8];     /* file system type */
            /* other fiealds are not added here */
        } fat16;

        struct {
            /* only used by FAT32 */
            __le32  length;     /* sectors/FAT */
            __le16  flags;      /* bit 8: fat mirroring,
                                   144                            low 4: active fat */
            __u8    version[2]; /* major, minor filesystem
                                   146                            version */
            __le32  root_cluster;   /* first cluster in
                                       148                            root directory */
            __le16  info_sector;    /* filesystem info sector */
            __le16  backup_boot;    /* backup boot sector */
            __le16  reserved2[6];   /* Unused */
            /* Extended BPB Fields for FAT32 */
            __u8    drive_number;   /* Physical drive number */
            __u8    state;          /* undocumented, but used
                                       155                            for mount state. */
            __u8    signature;  /* extended boot signature */
            __u8    vol_id[4];  /* volume ID */
            __u8    vol_label[11];  /* volume label */
            __u8    fs_type[8];     /* file system type */
            /* other fiealds are not added here */
        } fat32;
    };
};
/* This structure is filled with needed information for the subject from fat boot sector  */
struct fat_bios_param_block {
    __u16 fat_sector_size;
    __u8  fat_sec_per_clus;
    __u16 fat_reserved;
    __u8  fat_fats;
    __u16 fat_dir_entries;
    __u16 fat_sectors;
    __u16 fat_fat_length;
    __u32 fat_total_sect;

    __u8  fat16_state;
    __u32 fat16_vol_id;

    __u32 fat32_length;
    __u32 fat32_root_cluster;
    __u16 fat32_info_sector;
    __u8  fat32_state;
    __u32 fat32_vol_id;
};
/* This structure is msdos directory entry information */
struct msdos_dir_entry {
    __u8    name[MSDOS_NAME];/* name and extension */
    __u8    attr;       /* attribute bits */
    __u8    lcase;      /* Case for base and extension */
    __u8    ctime_cs;   /* Creation time, centiseconds (0-199) */
    __le16  ctime;      /* Creation time */
    __le16  cdate;      /* Creation date */
    __le16  adate;      /* Last access date */
    __le16  starthi;    /* High 16 bits of cluster in FAT32 */
    __le16  time,date,start;/* time, date and first cluster */
    __le32  size;       /* file size (in bytes) */
};

