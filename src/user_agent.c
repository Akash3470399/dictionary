#include <stdio.h>
#include "sqlite3.h"
#include "err_handler.h"

#define REQDB_FILE "req.db"
#define RESPDB_FILE "resp.db"

#define REQDB_CREATE_Q "create table if not exists request (id integer primary key, uid integer, query_word text);\0"
#define REQDB_INSERT_Q "insert into request values(?, ?, ?);\0"
#define RESPDB_READ_Q "\0"

typedef struct resp resp; 
typedef unsigned char uchar;

struct resp
{
    uchar status;
    int reqid;
    char *resptext;
};


sqlite3 *reqdb = NULL;
sqlite3 *respdb = NULL;

sqlite3_stmt *req_stmt = NULL;
sqlite3_stmt *resp_stmt = NULL;;

int db_setup();
int db_cleanup();
int query_word(int uid, char *word);
int check_ready_resp();
int id = 1;


int query_word(int uid, char *word)
{
    int rc;
    sqlite3_bind_int(req_stmt, 1, id);
    sqlite3_bind_int(req_stmt, 2, uid);
    sqlite3_bind_text(req_stmt, 3, word, -1, SQLITE_TRANSIENT);

    if((rc = sqlite3_step(req_stmt)) != SQLITE_DONE)
    {
        dump_err("db : word query failed", sqlite3_errstr(rc));
        return 1;
    }
    sqlite3_reset(req_stmt);
    return 0;
}

int db_setup()
{
    int rc;
    char *errstr;
    if((rc = sqlite3_open(REQDB_FILE, &reqdb)) != SQLITE_OK)
    {
        dump_err("db : request db connection failed", sqlite3_errstr(rc));
        return 1;
    }

    sqlite3_exec(reqdb, REQDB_CREATE_Q, NULL, NULL, &errstr);

    if(errstr != NULL)
    {
        dump_err("db : request db creation failed", errstr);
        sqlite3_free(errstr);
        sqlite3_close(reqdb);
        return 2;
    }

    if((rc = sqlite3_open(RESPDB_FILE, &respdb)) != SQLITE_OK)
    {
        dump_err("db : response db connection failed", sqlite3_errstr(rc));
        sqlite3_close(reqdb);
        return 3;
    }

    if((rc = sqlite3_prepare_v2(reqdb, REQDB_INSERT_Q, -1, &req_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : request prepare statment failed", sqlite3_errstr(rc));
        sqlite3_close(reqdb);
        sqlite3_close(respdb);
        return 4;
    }

    if((rc = sqlite3_prepare_v2(respdb, RESPDB_READ_Q, -1, &resp_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : response prepare statement failed", sqlite3_errstr(rc));
        sqlite3_finalize(req_stmt);
        sqlite3_close(reqdb);
        sqlite3_close(respdb);
    }
    
    return 0;
}


int main()
{   
    db_setup();
    query_word(124, "hello");
}
