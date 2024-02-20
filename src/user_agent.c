#include <stdio.h>
#include <stdlib.h>

#include "sqlite3.h"
#include "err_handler.h"


#define STRLIMT 200


#define COMMDB_FILE "comm.db"

#define REQDB_CREATE_Q "create table if not exists request (id integer primary key, uid integer, query_word text);\0"
#define RESPDB_CREATE_Q "create table if not exists response (id integer primary key, resp_text text);"

#define REQDB_INSERT_Q "insert into request values(?, ?, ?);\0"
#define RESPDB_READ_Q "select * from response where id == ?;\0"

typedef struct resp resp; 
typedef unsigned char uchar;

struct resp
{
    int reqid;
    char resptext[STRLIMT];
};


sqlite3 *commdb = NULL;

sqlite3_stmt *req_stmt = NULL;
sqlite3_stmt *resp_stmt = NULL;;

int db_setup();
int db_cleanup();
int query_word(int uid, char *word);
resp *check_resp(int id);
int id = 1;


int strncp(char *dst, const char *src)
{
    int count = 0;
    while(((*dst++ = *src++) != '\0') && count < STRLIMT)
        count += 1;
    return count; 
}


resp *check_resp(int id)
{
    resp *query_resp= NULL;
    int rc;
    const char *resptext; 
    sqlite3_bind_int(resp_stmt, 1, id);

    rc = sqlite3_step(resp_stmt);

    if(rc == SQLITE_ROW)
    {
        query_resp = (resp*)malloc(sizeof(resp));
        query_resp->reqid = sqlite3_column_int(resp_stmt, 0);
        resptext= sqlite3_column_text(resp_stmt, 1); 
        strncp(query_resp->resptext, resptext);        
    }
    else
        dump_err("db : response read failed", sqlite3_errstr(rc));
    sqlite3_reset(resp_stmt);
    return query_resp;
}
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

    if((rc = sqlite3_prepare_v2(commdb, REQDB_INSERT_Q, -1, &req_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : request prepare statment failed", sqlite3_errstr(rc));
        sqlite3_close(commdb);
        return 4;
    }

    if((rc = sqlite3_prepare_v2(commdb, RESPDB_READ_Q, -1, &resp_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : response prepare statement failed", sqlite3_errstr(rc));
        sqlite3_finalize(req_stmt);
        sqlite3_close(commdb);
        return 5;
    }
    return 0;
}


int main()
{  
    
    if(db_setup() > 0)
        return 1;
    resp *r1 = check_resp(1);

    if(r1)
    {
       printf("id %d, str %s\n", r1->reqid, r1->resptext);
    }
    else
        printf("failed\n");
        
    //query_word(124, "hello");
    free(r1);
    db_cleanup();
}
