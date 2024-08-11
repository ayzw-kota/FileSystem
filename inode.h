#ifndef INODE_H
#define INODE_H

#include <stdbool.h>
#include <stdint.h>
#include "blockdevice.h"
#include "table.h"

#define BLOCK_SIZE 512
#define INODE_SIZE 32
#define DIR_SIZE 16

typedef struct {
    uint16_t i_mode;
    uint8_t i_nlink;
    uint8_t i_uid;
    uint8_t i_gid;
    uint8_t i_size0;
    uint16_t i_size1;
    uint16_t i_addr[8];
    uint16_t i_atime[2];
    uint16_t i_mtime[2];
    uint32_t size;
    bool directory;
    bool large;
    Table* dir_table;
} Inode;

typedef struct {
    uint16_t s_isize;
    uint16_t s_fsize;
    uint16_t s_nfree;
    uint16_t s_free[100];
    uint16_t s_ninode;
    uint16_t s_inode[100];
    uint8_t s_flock;
    uint8_t s_ilock;
    uint8_t s_fmod;
    uint8_t s_ronly;
    uint16_t s_time[2];
    uint16_t pad[50];
} FilSys;

Inode** parse_inodes(BlockDevice* blockdevice, uint16_t size);
void destroy_inodes(Inode** nodes, int count);
void create_index(Inode* node, BlockDevice* blockdevice);
char* get_child(Inode* node);
char* get_child_option(Inode* node, Inode** nodes);
int get_node_id(Inode* node, const char* key);
FilSys* parse_filsys(BlockDevice* blockdevice);

#endif // INODE_H
