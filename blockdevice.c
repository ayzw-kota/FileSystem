#include "blockdevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BlockDevice* create_blockdevice(const char* path) {
    BlockDevice* blockdevice = (BlockDevice*)malloc(sizeof(BlockDevice));
    if (!blockdevice) {
        fprintf(stderr, "Failed to allocate memory for BlockDevice.\n");
        return NULL;
    }

    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Cannot open file: %s\n", path);
        free(blockdevice);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    blockdevice->data = (uint8_t*)malloc(file_size);
    if (!blockdevice->data) {
        fprintf(stderr, "Failed to allocate memory for BlockDevice data.\n");
        fclose(file);
        free(blockdevice);
        return NULL;
    }

    fread(blockdevice->data, 1, file_size, file);
    fclose(file);

    blockdevice->path = strdup(path);
    blockdevice->p = 0;

    return blockdevice;
}

void destroy_blockdevice(BlockDevice* blockdevice) {
    if (blockdevice) {
        free(blockdevice->data);
        free((void*)blockdevice->path);
        free(blockdevice);
    }
}

void set_index(BlockDevice* blockdevice, uint32_t p) {
    blockdevice->p = p;
}

uint16_t read_short(BlockDevice* blockdevice) {
    uint8_t tmp = blockdevice->data[blockdevice->p++];
    return (uint16_t)(tmp | blockdevice->data[blockdevice->p++] << 8);
}

char* read_string(BlockDevice* blockdevice) {
    char* s = (char*)&blockdevice->data[blockdevice->p];
    blockdevice->p += (strlen(s) + 1);
    return strdup(s);
}
