#ifndef DBINIT_H
#define DBINIT_H

#include <sqlite3.h>

typedef struct{
    char* name;
    char* link;
    int episode_count;
    int id;
} audiobook;

extern sqlite3* db;

void init();

#endif
