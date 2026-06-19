#include "./func.h"
#include "../error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

extern sqlite3* db;

int add_audiobook(const char* name,const int episode_count,const char* link){
    if (!db) return SQLITE_ERROR;

    sqlite3_stmt* stmt = NULL;
    const char* query = "insert into audiobooks (name,episode_count,link) values (?,?,?)";
    int rc = sqlite3_prepare_v2(db,query,-1,&stmt,NULL);
    raise(rc,"failed to prepare statement",db);

    rc = sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) goto done;

    rc = sqlite3_bind_int(stmt, 2, episode_count);
    if (rc != SQLITE_OK) goto done;

    rc = sqlite3_bind_text(stmt, 3, link, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) goto done;

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_DONE) {
        printf("Row inserted successfully!\n");
        sqlite3_int64 new_id = sqlite3_last_insert_rowid(db);
        printf("Generated Row ID: %lld\n", new_id);
    } else {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }
done:
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

audiobook* get_audiobooks(){
    if (!db) return NULL;

    const char* query = "select id,name,episode_count,link from audiobooks";
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    raise(rc,"failed to prepare select all statement",db);

    size_t cap = 8;
    size_t len = 0;
    audiobook* list = malloc(sizeof(audiobook) * cap);
    if (!list) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (len >= cap) {
            cap *= 2;
            audiobook* tmp = realloc(list, sizeof(audiobook) * cap);
            if (!tmp) { 
                for (size_t i = 0; i < len; ++i) {
                    free(list[i].name);
                    free(list[i].link);
                }
                free(list);
                sqlite3_finalize(stmt);
                return NULL;
            }
            list = tmp;
        }

        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* link = sqlite3_column_text(stmt, 3);
        list[len].id = sqlite3_column_int(stmt, 0);
        list[len].episode_count = sqlite3_column_int(stmt, 2);
        list[len].name = name ? strdup((const char*)name) : NULL;
        list[len].link = link ? strdup((const char*)link) : NULL;
        ++len;
    }

    if (rc != SQLITE_DONE) {
        for (size_t i = 0; i < len; ++i) {
            free(list[i].name);
            free(list[i].link);
        }
        free(list);
        sqlite3_finalize(stmt);
        return NULL;
    }

    sqlite3_finalize(stmt);

    audiobook* out = realloc(list, sizeof(audiobook) * (len + 1));
    if (!out) {
        return list;
    }
    out[len].id = -1;
    out[len].name = NULL;
    out[len].link = NULL;
    out[len].episode_count = 0;
    return out;
}

audiobook get_audiobook_by_id(int id){
    audiobook empty = {NULL, NULL, 0, -1};
    if (!db) return empty;

    const char* query = "select id,name,episode_count,link from audiobooks where id = ?";
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    raise(rc,"failed to prepare select by id statement",db);

    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        audiobook book;
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* link = sqlite3_column_text(stmt, 3);
        book.id = sqlite3_column_int(stmt, 0);
        book.episode_count = sqlite3_column_int(stmt, 2);
        book.name = name ? strdup((const char*)name) : NULL;
        book.link = link ? strdup((const char*)link) : NULL;
        sqlite3_finalize(stmt);
        return book;
    }

    sqlite3_finalize(stmt);
    return empty;
}

int update_audiobook_by_id(const audiobook* book){
    if (!db || !book) return SQLITE_ERROR;

    const char* query = "update audiobooks set name = ?, episode_count = ?, link = ? where id = ?";
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    raise(rc,"failed to prepare update statement",db);

    sqlite3_bind_text(stmt,1,book->name,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,2,book->episode_count);
    sqlite3_bind_text(stmt,3,book->link,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,4,book->id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Update failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int delete_audiobook_by_id(const int id){
    if (!db) return SQLITE_ERROR;

    const char* query = "delete from audiobooks where id = ?";
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    raise(rc,"failed to prepare delete statement",db);

    sqlite3_bind_int(stmt,1,id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Delete failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}