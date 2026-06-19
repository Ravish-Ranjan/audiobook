#ifndef ERROR_H
#define ERROR_H

#include <sqlite3.h>

void raise(const int status,const char* msg,sqlite3* db);

#endif