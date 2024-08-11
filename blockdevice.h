#ifndef BLOCKDEVICE_H
#define BLOCKDEVICE_H

#include <stdint.h>

typedef struct {
    uint8_t *data;
    const char *path;
    uint32_t p;
} BlockDevice;

BlockDevice* create_blockdevice(const char* path);
void destroy_blockdevice(BlockDevice* blockdevice);
void set_index(BlockDevice* blockdevice, uint32_t p);
uint16_t read_short(BlockDevice* blockdevice);
char* read_string(BlockDevice* blockdevice);

#endif // BLOCKDEVICE_H
