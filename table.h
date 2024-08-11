#ifndef TABLE_H
#define TABLE_H

typedef struct {
    char* name;
    int inode_num;
} Entry;

typedef struct {
    Entry* entries;
    int count;
    int capacity;
} Table;

Table* create_dir_table();
void add_entry(Table* table, const char* name, int inode_num);
void destroy_dir_table(Table* table);

#endif // TABLE_H
