#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // ここを追加
#include "blockdevice.h"
#include "inode.h"
#include "table.h"

int main() {
    BlockDevice* blockdevice = create_blockdevice("./v6root");
    if (!blockdevice) {
        fprintf(stderr, "Block device creation failed.\n");
        return EXIT_FAILURE;
    }

    FilSys* fs = parse_filsys(blockdevice);
    if (!fs) {
        fprintf(stderr, "Failed to parse file system.\n");
        destroy_blockdevice(blockdevice);
        return EXIT_FAILURE;
    }

    Inode** nodes = parse_inodes(blockdevice, fs->s_isize);
    if (!nodes) {
        fprintf(stderr, "Failed to parse inodes.\n");
        free(fs);
        destroy_blockdevice(blockdevice);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < fs->s_isize * BLOCK_SIZE / INODE_SIZE; i++) {
        if (nodes[i]->directory) {
            create_index(nodes[i], blockdevice);
        }
    }

    uint32_t nid = 1;
    char cmd[256];
    char* tokens[256];
    int token_count;

    while (1) {
        printf(">> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;  // strcspn 関数を使用

        token_count = 0;
        char* token = strtok(cmd, " ");  // strtok 関数を使用
        while (token != NULL) {
            tokens[token_count++] = token;
            token = strtok(NULL, " ");  // strtok 関数を使用
        }

        if (strcmp(tokens[0], "cd") == 0) {  // strcmp 関数を使用
            Inode* node = nodes[nid];
            if (node->directory) {
                int new_nid = get_node_id(node, tokens[1]);
                if (new_nid != -1) {
                    nid = new_nid;
                } else {
                    fprintf(stderr, "Directory not found: %s\n", tokens[1]);
                }
            }
        } else if (strcmp(tokens[0], "ls") == 0) {
            Inode* node = nodes[nid];
            if (token_count == 1 && node->directory) {
                char* child = get_child(node);
                if (child) {
                    printf("%s", child);
                    free(child);
                }
            } else if (token_count == 2 && node->directory) {
                if (strcmp(tokens[1], "-l") == 0) {
                    char* child_option = get_child_option(node, nodes);
                    if (child_option) {
                        printf("%s", child_option);
                        free(child_option);
                    }
                }
            }
        }
    }

    destroy_inodes(nodes, fs->s_isize * BLOCK_SIZE / INODE_SIZE);
    free(fs);
    destroy_blockdevice(blockdevice);
    return EXIT_SUCCESS;
}
