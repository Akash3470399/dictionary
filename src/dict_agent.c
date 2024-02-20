#include <stdio.h>

#include "sqlite3.h"
#include "cmptrie.h"
#include "err_handler.h"

#define COMMDB_FILE "comm.db"
#define REQDB_CREATE_Q "create table if not exists request (id integer primary key, uid integer, query_word text);\0"
#define RESPDB_CREATE_Q "create table if not exists response (id integer primary key, resp_text text);"

#define REQDB_READ_Q "select * from request where id > ? ;\0"
#define RESPDB_INSERT_Q "insert into response values ( ? , ? );\0"

sqlite3 *commdb = NULL;

sqlite3_stmt *req_stmt = NULL;
sqlite3_stmt *resp_stmt = NULL;;


int db_cleanup()
{
    int rc;
    rc = sqlite3_finalize(req_stmt);
    rc |= sqlite3_finalize(resp_stmt);
    rc |= sqlite3_close(commdb);

    return rc;
}

int db_setup()
{
    int rc;
    char *errstr;
    if((rc = sqlite3_open(COMMDB_FILE, &commdb)) != SQLITE_OK)
    {
        dump_err("db : request db connection failed", sqlite3_errstr(rc));
        return 1;
    }

    sqlite3_exec(commdb, REQDB_CREATE_Q, NULL, NULL, &errstr);
    if(errstr != NULL)
    {
        dump_err("db : request table creation failed", errstr);
        sqlite3_free(errstr);
        sqlite3_close(commdb);
        return 2;
    }

    sqlite3_exec(commdb, RESPDB_CREATE_Q, NULL, NULL, &errstr);
    if(errstr != NULL)
    {
        dump_err("db : response table creation failed", errstr);
        sqlite3_free(errstr);
        sqlite3_close(commdb);
        return 3;
    }

    if((rc = sqlite3_prepare_v2(commdb, REQDB_READ_Q, -1, &req_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : request prepare statment failed", sqlite3_errstr(rc));
        sqlite3_close(commdb);
        return 4;
    }

    if((rc = sqlite3_prepare_v2(commdb, RESPDB_INSERT_Q, -1, &resp_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : response prepare statement failed", sqlite3_errstr(rc));
        sqlite3_finalize(req_stmt);
        sqlite3_close(commdb);
        return 5;
    }
    return 0;
}

int check_word_query(int lastid)
{
    int rc, reqid;
    const char *queried_word, *meaning = "hello there";


    if(sqlite3_bind_int(req_stmt, 1, lastid) != SQLITE_OK)
        printf("bind fail\n");

    rc = sqlite3_step(req_stmt);

    if(rc == SQLITE_ROW)
    {
        reqid = sqlite3_column_int(req_stmt, 0);
        queried_word = sqlite3_column_text(req_stmt, 2);

        int is_present = is_word_present((char *)queried_word);

        if(is_present > 0)
        {
            sqlite3_bind_int(resp_stmt, 1, reqid);
            sqlite3_bind_text(resp_stmt, 2, meaning, -1, SQLITE_TRANSIENT);

            sqlite3_step(resp_stmt);
            sqlite3_reset(resp_stmt);    
        }
    }
    else
        printf("s %d\n", SQLITE_DONE == rc);
    sqlite3_reset(req_stmt);
}

int main()
{
    db_setup();
    if(init_rfd() > 0)
    {
        check_word_query(3);        
    
    }
    else
        printf("exit\n");
    db_cleanup();
}
