#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "fat_info.h"

void parse_bpb(struct fat_boot_sector *b, struct fat_bios_param_block *bpb);
void print_fat_table(char* buf, unsigned int table_size);
void print_cluster(char* buf,unsigned int cluster_size);
void print_deleted_de(struct msdos_dir_entry *de);
void read_disk(FILE *disk, char* buf, unsigned int offset, unsigned int size);

int main (unsigned int argc, char** argv)
{
    FILE *disk;
    __u32 eof = EOF_FAT32;
    __u32 target_cluster_num;
    unsigned int i,j;
    unsigned int table_start, table_size, table_end;
    unsigned int root_clus, target_clus;
    unsigned int cluster_size;
    char *bs_buf, *table_buf, *cluster_buf, *cluster_buf2;
    struct fat_bios_param_block bpb;
    struct msdos_dir_entry *de;

    if(argc < 2)
    {
        printf("no filename\n");
        return -1;
    }

    disk = fopen("/dev/rxd0","r+");

    //-----  read boot_sector
    bs_buf = (char*)malloc(SECTOR_SIZE);
    read_disk(disk,bs_buf,0,SECTOR_SIZE);

    //----- boot_secter --> bios_param_block
    parse_bpb((struct fat_boot_sector*)bs_buf,&bpb);

    //----- print_fat_table
    table_start = bpb.fat_sector_size * bpb.fat_reserved;
    table_size = bpb.fat_sector_size*bpb.fat32_length;
    table_end = table_start + (table_size * bpb.fat_fats);
    table_buf = (char*)malloc(table_size);

    read_disk(disk,table_buf,table_start,table_size);

    //----- print_root_cluster
    cluster_size = bpb.fat_sector_size * bpb.fat_sec_per_clus;
    root_clus = table_end + (bpb.fat32_root_cluster - CLUS_START_NUM) * cluster_size;
    cluster_buf = (char*)malloc(cluster_size);
    cluster_buf2 = (char*)malloc(cluster_size);
    
    read_disk(disk,cluster_buf,root_clus,cluster_size);

    //----- recovery
    for(de=(struct msdos_dir_entry*)cluster_buf, j=0; sizeof(struct msdos_dir_entry)*j < cluster_size; de++, j++)
    {
        if(de->name[0] == DELETED_FLAG)
        {
            print_deleted_de(de);

            if(!memcmp(argv[1],&de->name[1],strlen(argv[1])))
            {
                printf("START RECOVERY\n");

                //-----print_target_cluster
                target_cluster_num = de->starthi << 16;
                target_cluster_num += de->start;
                target_clus = table_end + (target_cluster_num - CLUS_START_NUM) * cluster_size;
                
                read_disk(disk,cluster_buf2,target_clus,cluster_size);

                printf("-targetfile_cluster-\n");
                print_cluster(cluster_buf2,cluster_size);

                //----recovery name
                for(i=0 ; i<MSDOS_NAME-1; i++)
                    de->name[i] = de->name[i+1];
                fseek(disk, root_clus + sizeof(struct msdos_dir_entry)*j, SEEK_SET);
                fwrite(de, sizeof(struct msdos_dir_entry), 1, disk);

                //----recovery fatentry
                fseek(disk, table_start + target_cluster_num*sizeof(__u32), SEEK_SET);
                fwrite(&eof,sizeof(__u32),1,disk);

                printf("success\n");
                break;
            }
        }
    }
    if(sizeof(struct msdos_dir_entry)*j >= cluster_size)
        printf("fail\n");

    free(bs_buf);
    free(table_buf);
    free(cluster_buf);
    free(cluster_buf2);
    fclose(disk);

    return 0;
}

void parse_bpb(struct fat_boot_sector *b, struct fat_bios_param_block *bpb)
{
    memset(bpb, 0, sizeof(*bpb));
    memcpy(&bpb->fat_sector_size,&b->sector_size,sizeof(__u16));
    bpb->fat_sec_per_clus = b->sec_per_clus;
    memcpy(&bpb->fat_reserved,&b->reserved,sizeof(__u16));
    bpb->fat_fats = b->fats;
    memcpy(&bpb->fat_dir_entries,&b->dir_entries,sizeof(__u16));
    memcpy(&bpb->fat_sectors,&b->sectors,sizeof(__u16));
    memcpy(&bpb->fat_fat_length,&b->fat_length,sizeof(__u16));
    memcpy(&bpb->fat_total_sect,&b->total_sect,sizeof(__u32));
    bpb->fat16_state = b->fat16.state;
    memcpy(&bpb->fat16_vol_id,&b->fat16.vol_id,sizeof(__u32));
    memcpy(&bpb->fat32_length,&b->fat32.length,sizeof(__u32));
    memcpy(&bpb->fat32_root_cluster,&b->fat32.root_cluster,sizeof(__u32));
    memcpy(&bpb->fat32_info_sector,&b->fat32.info_sector,sizeof(__u32));
    bpb->fat32_state = b->fat32.state;
    memcpy(&bpb->fat32_vol_id,&b->fat32.vol_id,sizeof(__u32));
}

void print_fat_table(char* buf, unsigned int table_size)
{
    __u32 *entry;

    for(entry=(__u32*)buf; table_size; table_size-=sizeof(__u32),entry++)
    {
        if(*entry == 268435455)
            printf("EOF|");
        else 
            printf("%d|",*entry);
    }

    printf("\n");
}

void print_cluster(char* buf,unsigned int cluster_size)
{
    unsigned int i;

    for(i=0;i<cluster_size;i++)
        printf("%c",buf[i]);

    printf("\n");
}

void print_deleted_de(struct msdos_dir_entry *de)
{
    printf("-DELETED_msdos_dir_entry-:\n");
    printf("de->name:%s |",&de->name[1]);
    printf(" de->attr:%u |",de->attr);
    printf(" de->start_hi:%u |",de->starthi);
    printf(" de->start:%u |",de->start);
    printf(" de->size:%x |\n",de->size);
}

void read_disk(FILE *disk, char* buf, unsigned int offset, unsigned int size)
{
    fseek(disk,offset,SEEK_SET);
    fread(buf,size,1,disk);
}
