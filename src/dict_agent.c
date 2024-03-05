#include <stdio.h>
#include <unistd.h>
#include "sqlite3.h"

#include "cmptrie.h"
#include "err_handler.h"

#define COMMDB_FILE "comm.db"
#define STRLEN 200

#define REQDB_CREATE_Q "create table if not exists %s (reqid integer primary key, query_word text);"
#define RESPDB_CREATE_Q "create table if not exists %s (respid integer primary key, resp_text text);"

#define REQDB_READ_Q "select * from %s where reqid > ? ;"
#define RESPDB_INSERT_Q "insert into %s values ( ? , ? );"

sqlite3 *commdb = NULL;

sqlite3_stmt *req_stmt = NULL;
sqlite3_stmt *resp_stmt = NULL;;
refdata_info *rfd;

char dbquery_str[STRLEN];
int cur_reqid = 0;

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
        dump_err("db : table names not given", "");
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

    sprintf(dbquery_str, REQDB_READ_Q, reqtable);
    if((rc = sqlite3_prepare_v2(commdb, dbquery_str, -1, &req_stmt, NULL)) != SQLITE_OK)
    {
        dump_err("db : request prepare statment failed", sqlite3_errstr(rc));
        sqlite3_close(commdb);
        return 4;
    }
    
    sprintf(dbquery_str, RESPDB_INSERT_Q, resptable);
    if((rc = sqlite3_prepare_v2(commdb, dbquery_str, -1, &resp_stmt, NULL)) != SQLITE_OK)
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
    int is_present = 0;
    char  meaning[1000];
    const uchar *queried_word;
    sqlite3_bind_int(req_stmt, 1, lastid);

    while((rc = sqlite3_step(req_stmt))  == SQLITE_ROW)
    {
        reqid = sqlite3_column_int(req_stmt, 0);
        queried_word = sqlite3_column_text(req_stmt, 1);
        is_present = get_meaning(rfd, (char*) queried_word, meaning);

        if(is_present > 0)
        {
            sqlite3_bind_int(resp_stmt, 1, reqid);
            sqlite3_bind_text(resp_stmt, 2, meaning, -1, SQLITE_TRANSIENT);
	    cur_reqid = (reqid > cur_reqid)? reqid : cur_reqid;
            sqlite3_step(resp_stmt);
            sqlite3_reset(resp_stmt);    
        }
    }
    sqlite3_reset(req_stmt);
    return is_present;
}



// argv[1] : request table name
// argv[2] : response table name
// argv[3] : refdata file name
int main(int argc, char *argv[])
{
    int reqid = 0;
    if(db_setup(argv[1], argv[2]) == 0)
    {
        if(init_refdata_info("data/rfd", &rfd) == 0)
        {
            while(1)
            {
                check_word_query(cur_reqid);        
	        	sleep(2);
            }
        }
        else
        printf("unable to find refdata\n"); 
    }
        db_cleanup();
    return 0;
}
