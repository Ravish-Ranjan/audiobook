#include "./error.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

void raise(int status,const char msg[],sqlite3 *db){
    if(status != SQLITE_OK){
        fprintf(stderr,"DB Error : %s\n",msg);
        sqlite3_close(db);
        exit(1);
    }
}