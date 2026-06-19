#include "./init.h"
#include "../error.h"
#include <stdio.h>
#include <string.h>

sqlite3* db;

void init(){
    char* err_msg;

    raise(sqlite3_open("./db/audiobooks.db",&db),"Error : error connecting to the database",db);

    const char* create_table_query = "create table if not exists audiobooks(id integer primary key, name text not null,episode_count integer not null, link text not null )";

    raise(sqlite3_exec(db,create_table_query,0,0,&err_msg),err_msg,db);
}