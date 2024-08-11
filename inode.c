#include "inode.h"
#include "blockdevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Inode** parse_inodes(BlockDevice* blockdevice, uint16_t size) {
    int max_inodes = BLOCK_SIZE * size / INODE_SIZE;
    Inode** nodes = (Inode**)malloc(sizeof(Inode*) * (max_inodes + 1));
    if (!nodes) {
        fprintf(stderr, "Failed to allocate memory for inodes.\n");
        return NULL;
    }

    set_index(blockdevice, 2 * BLOCK_SIZE);
    nodes[0] = (Inode*)calloc(1, sizeof(Inode));
    if (!nodes[0]) {
        fprintf(stderr, "Failed to allocate memory for root inode.\n");
        free(nodes);
        return NULL;
    }

    for (int i = 1; i <= max_inodes; i++) {
        Inode* node = (Inode*)malloc(sizeof(Inode));
        if (!node) {
            fprintf(stderr, "Failed to allocate memory for inode %d.\n", i);
            destroy_inodes(nodes, i - 1);
            return NULL;
        }

        node->i_mode = read_short(blockdevice);
        node->i_nlink = blockdevice->data[blockdevice->p++];
        node->i_uid = blockdevice->data[blockdevice->p++];
        node->i_gid = blockdevice->data[blockdevice->p++];
        node->i_size0 = blockdevice->data[blockdevice->p++];
        node->i_size1 = read_short(blockdevice);

        for (int j = 0; j < 8; j++) {
            node->i_addr[j] = read_short(blockdevice);
        }

        for (int j = 0; j < 2; j++) {
            node->i_atime[j] = read_short(blockdevice);
        }

        for (int j = 0; j < 2; j++) {
            node->i_mtime[j] = read_short(blockdevice);
        }

        node->size = (node->i_size0 & 0xff) << 16 | (node->i_size1 & 0xffff);
        node->directory = (((node->i_mode >> 14) & 1) == 1) && (((node->i_mode >> 13) & 1) != 1);
        node->large = ((node->i_mode >> 12) & 1) == 1;
        node->dir_table = NULL;
        nodes[i] = node;
    }
    return nodes;
}

void destroy_inodes(Inode** nodes, int count) {
    for (int i = 0; i <= count; i++) {
        if (nodes[i]->dir_table) {
            destroy_dir_table(nodes[i]->dir_table);
        }
        free(nodes[i]);
    }
    free(nodes);
}

void create_index(Inode* node, BlockDevice* blockdevice) {
    if (!node->directory) return;
    node->dir_table = create_dir_table();
    if (!node->dir_table) {
        fprintf(stderr, "Failed to create directory table for inode.\n");
        return;
    }

    for (int i = 0; i < 8; i++) {
        uint32_t offset = node->i_addr[i] * BLOCK_SIZE;
        for (int j = 0; j < BLOCK_SIZE / DIR_SIZE; j++) {
            if (j >= (node->size - (i * BLOCK_SIZE)) / DIR_SIZE) {
                return;
            }
            set_index(blockdevice, j * DIR_SIZE + offset);
            int ino_num = read_short(blockdevice);
            char* name = read_string(blockdevice);
            if (ino_num != 0) {
                add_entry(node->dir_table, name, ino_num);
            }
            free(name);
        }
    }
}

char* get_child(Inode* node) {
    if (!node->directory || !node->dir_table) return strdup("");
    size_t total_length = 0;
    for (int i = 0; i < node->dir_table->count; i++) {
        total_length += strlen(node->dir_table->entries[i].name) + 1;
    }
    char* result = (char*)malloc(total_length + 1);
    if (!result) {
        fprintf(stderr, "Failed to allocate memory for child list.\n");
        return NULL;
    }

    result[0] = '\0';
    for (int i = 0; i < node->dir_table->count; i++) {
        strcat(result, node->dir_table->entries[i].name);
        strcat(result, "\n");
    }
    return result;
}

char* get_child_option(Inode* node, Inode** nodes) {
    if (!node->directory || !node->dir_table) return strdup("");
    size_t total_length = 0;
    for (int i = 0; i < node->dir_table->count; i++) {
        total_length += 128;
    }
    char* result = (char*)malloc(total_length);
    if (!result) {
        fprintf(stderr, "Failed to allocate memory for detailed child list.\n");
        return NULL;
    }

    result[0] = '\0';
    char line[128];
    for (int i = 0; i < node->dir_table->count; i++) {
        Inode* child = nodes[node->dir_table->entries[i].inode_num];
        char gx = (child->i_mode & 0x08) ? 'x' : '-';
        char or = (child->i_mode & 0x04) ? 'r' : '-';
        char ow = (child->i_mode & 0x02) ? 'w' : '-';
        char ox = (child->i_mode & 0x01) ? 'x' : '-';
        char ur = (child->i_mode & 0x100) ? 'r' : '-';
        char uw = (child->i_mode & 0x80) ? 'w' : '-';
        char ux = (child->i_mode & 0x40) ? 'x' : '-';
        char gr = (child->i_mode & 0x20) ? 'r' : '-';
        char gw = (child->i_mode & 0x10) ? 'w' : '-';
        char dir = child->directory ? 'd' : '-';
        snprintf(line, sizeof(line), "%c%c%c%c%c%c%c%c%c%c %8d %s\n",
                 dir, ur, uw, ux, gr, gw, gx, or, ow, ox,
                 child->size, node->dir_table->entries[i].name);
        strcat(result, line);
    }
    return result;
}

int get_node_id(Inode* node, const char* key) {
    if (!node->directory || !node->dir_table) return -1;
    for (int i = 0; i < node->dir_table->count; i++) {
        if (strcmp(node->dir_table->entries[i].name, key) == 0) {
            return node->dir_table->entries[i].inode_num;
        }
    }
    return -1;
}

FilSys* parse_filsys(BlockDevice* blockdevice) {
    FilSys* fs = (FilSys*)malloc(sizeof(FilSys));
    if (!fs) {
        fprintf(stderr, "Failed to allocate memory for FilSys.\n");
        return NULL;
    }

    set_index(blockdevice, BLOCK_SIZE);
    fs->s_isize = read_short(blockdevice);
    fs->s_fsize = read_short(blockdevice);
    fs->s_nfree = read_short(blockdevice);
    for (int i = 0; i < 100; i++) {
        fs->s_free[i] = read_short(blockdevice);
    }
    fs->s_ninode = read_short(blockdevice);
    for (int i = 0; i < 100; i++) {
        fs->s_inode[i] = read_short(blockdevice);
    }
    fs->s_flock = blockdevice->data[blockdevice->p++];
    fs->s_ilock = blockdevice->data[blockdevice->p++];
    fs->s_fmod = blockdevice->data[blockdevice->p++];
    fs->s_ronly = blockdevice->data[blockdevice->p++];
    for (int i = 0; i < 2; i++) {
        fs->s_time[i] = read_short(blockdevice);
    }
    for (int i = 0; i < 50; i++) {
        fs->pad[i] = read_short(blockdevice);
    }
    return fs;
}

