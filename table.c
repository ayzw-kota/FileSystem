#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Table* create_dir_table() {
    Table* table = (Table*)malloc(sizeof(Table));
    if (!table) {
        fprintf(stderr, "Failed to allocate memory for Table.\n");
        return NULL;
    }

    table->entries = NULL;
    table->count = 0;
    table->capacity = 0;
    return table;
}

void add_entry(Table* table, const char* name, int inode_num) {
    if (table->count == table->capacity) {
        int new_capacity = table->capacity == 0 ? 1 : table->capacity * 2;
        Entry* new_entries = (Entry*)realloc(table->entries, new_capacity * sizeof(Entry));
        if (!new_entries) {
            fprintf(stderr, "Failed to reallocate memory for entries.\n");
            return;
        }
        table->entries = new_entries;
        table->capacity = new_capacity;
    }

    table->entries[table->count].name = strdup(name);
    if (!table->entries[table->count].name) {
        fprintf(stderr, "Failed to duplicate entry name.\n");
        return;
    }

    table->entries[table->count].inode_num = inode_num;
    table->count++;
}

void destroy_dir_table(Table* table) {
    if (!table) return;

    for (int i = 0; i < table->count; i++) {
        free(table->entries[i].name);
    }
    free(table->entries);
    free(table);
}
