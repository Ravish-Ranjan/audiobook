#ifndef DBFUNC_H
#define DBFUNC_H

#include "./init.h"

// add new audiobook
int add_audiobook(const char* name,const int episode_count,const char* link);

// get all audiobooks names
audiobook* get_audiobooks();

// get info of a audiobook by id
audiobook get_audiobook_by_id(int id);

// update audiobook info my id
int update_audiobook_by_id(const audiobook* book);

// delete audiobook my id
int delete_audiobook_by_id(const int id);


#endif