#include <stdio.h>
#include <stdlib.h>

#include "sqlite3.h"
#include "cmptrie.h"
#include "err_handler.h"


#define STRLEN 200

#define COMMDB_FILE "comm.db"

#define REQDB_CREATE_Q "create table if not exists %s (reqid integer primary key, query_word text);"
#define RESPDB_CREATE_Q "create table if not exists %s (respid integer primary key, resp_text text);"

#define REQDB_INSERT_Q "insert into %s values(?, ?);"
#define RESPDB_READ_Q "select * from %s where respid == ?;"

typedef struct resp resp; 
typedef unsigned char uchar;

struct resp
{
    int reqid;
    char resptext[STRLEN];
};


sqlite3 *commdb = NULL;

sqlite3_stmt *req_stmt = NULL;
sqlite3_stmt *resp_stmt = NULL;;

char dbquery_str[STRLEN];


int db_setup();
int db_cleanup();
int query_word(char *word);
resp check_resp(int id);
<<<<<<< HEAD
int id = 3;
=======
int id = 0;
>>>>>>> 0b07f57 (dict_agent, user_agent with prepare statement with one request id done.)


int strncp(char *dst, char *src)
{
    int count = 0;
    while(((*dst++ = *src++) != '\0') && count < STRLEN)
        count += 1;
    return count; 
}


resp check_resp(int reqid)
{
    resp query_resp = {0, 0};
    int rc;
    const uchar *resptext; 

    sqlite3_bind_int(resp_stmt, 1, reqid);

    if((rc = sqlite3_step(resp_stmt)) == SQLITE_ROW)
    {
        query_resp.reqid = sqlite3_column_int(resp_stmt, 0);
        resptext = sqlite3_column_text(resp_stmt, 1); 
        strncp(query_resp.resptext, (char*)resptext);       

        printf("got %s\n", resptext);
    }
    sqlite3_reset(resp_stmt);
    return query_resp;
}


int query_word(char *word)
{
    int rc;
    sqlite3_bind_int(req_stmt, 1, id++);
    sqlite3_bind_text(req_stmt, 2, word, -1, SQLITE_TRANSIENT);

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
int db_setup(char *reqtable, char *resptable)
{
    int rc;
    char *errstr;

    if(!reqtable || !resptable)
    {
        dump_err("db : tables names not given ", "");
        return 6;
    }

    if((rc = sqlite3_open(COMMDB_FILE, &commdb)) != SQLITE_OK)
    {
        dump_err("db : request db connection failed", sqlite3_errstr(rc));
        return 1;
    }

    sprintf(dbquery_str, REQDB_CREATE_Q, reqtable);
    sqlite3_exec(commdb, dbquery_str, NULL, NULL, &errstr);
    if(errstr != NULL)
    {
        dump_err("db : request table creation failed", errstr);
        sqlite3_free(errstr);
        sqlite3_close(commdb);
        return 2;
    }

    sprintf(dbquery_str, RESPDB_CREATE_Q, resptable);
    sqlite3_exec(commdb, dbquery_str, NULL, NULL, &errstr);
    if(errstr != NULL)
    {
        dump_err("db : response table creation failed", errstr);
        sqlite3_free(errstr);
        sqlite3_close(commdb);
        return 3;
    }

    sprintf(dbquery_str, REQDB_INSERT_Q, reqtable);
    if((rc = sqlite3_prepare_v2(commdb, dbquery_str, -1, &req_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : request prepare statment failed", sqlite3_errstr(rc));
        sqlite3_close(commdb);
        return 4;
    }

    sprintf(dbquery_str, RESPDB_READ_Q, resptable);
    if((rc = sqlite3_prepare_v2(commdb, dbquery_str, -1, &resp_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : response prepare statement failed", sqlite3_errstr(rc));
        sqlite3_finalize(req_stmt);
        sqlite3_close(commdb);
        return 5;
    }
    return 0;
}


int main(int argc, char *argv[])
{  
    char str[100] = "hello";
    if(db_setup(argv[1], argv[2]) > 0)
        return 1;

    int rid = 0;

    while(str[0] != '0')
    {
        query_word(str);
        check_resp(rid);

        printf("q :");
        fscanf(stdin, "%s\n", str);
        printf("resp id : ");
        scanf("%d", &rid);
    }
    db_cleanup();
}
