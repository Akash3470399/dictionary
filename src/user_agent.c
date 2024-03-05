#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


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
int cur_reqid = 1;

char dbquery_str[STRLEN];


int db_setup();
int db_cleanup();
int query_word(char *word);
resp check(int id);


int strncp(char *dst, char *src)
{
    int count = 0;
    while(((*dst++ = *src++) != '\0') && count < STRLEN)
        count += 1;
    return count; 
}


resp check(int reqid)
{
    resp query = {0, 0};
    int rc;
    const uchar *resptext; 

    sqlite3_bind_int(resp_stmt, 1, reqid);

    if((rc = sqlite3_step(resp_stmt)) == SQLITE_ROW)
    {
        query.reqid = sqlite3_column_int(resp_stmt, 0);
        resptext = sqlite3_column_text(resp_stmt, 1); 
        strncp(query.resptext, (char*)resptext);       

        printf("got %s\n", resptext);
    }
    sqlite3_reset(resp_stmt);
    return query;
}


int query_word(char *word)
{
    int rc, reqid = -1;
    sqlite3_bind_int(req_stmt, 1, cur_reqid);
    sqlite3_bind_text(req_stmt, 2, word, -1, SQLITE_TRANSIENT);

    if((rc = sqlite3_step(req_stmt)) == SQLITE_DONE)
        reqid = cur_reqid++;
    else
        dump_err("db : word query failed", sqlite3_errstr(rc));
    
    sqlite3_reset(req_stmt);
    return reqid;
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
    resp query_resp;
    char buffer[1000] = "s";
    struct sockaddr_in selfaddr, clientaddr;
    int self_sockfd, port = 6666, client_sockfd, client_addrlen = 0;
    int reqid, n;
    
    if(db_setup(argv[1], argv[2]) > 0)
        return 1;

    if((self_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket createion failed\n");
        return 1;
    }
    
    memset(&selfaddr, 0, sizeof(selfaddr));
    memset(&clientaddr, 0,sizeof(clientaddr));


    selfaddr.sin_family = AF_INET;
    selfaddr.sin_port = htons(port);
    selfaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(self_sockfd, (struct sockaddr *)&selfaddr, sizeof(selfaddr)) < 0)
    {
        printf("bind failed\n");
        close(self_sockfd);
        return 1;
    }

    if(listen(self_sockfd, 5) < 0)
    {
        printf("listen failed\n");
        close(self_sockfd);
        return 1;
    }

    while(buffer[0] != 'e')
    {
        if((client_sockfd = accept(self_sockfd, NULL, NULL)) > 0)
        {

            memset(buffer, 0, sizeof(buffer));
            if((n = read(client_sockfd, buffer, sizeof(buffer))) > 0)
            {
                buffer[n] = '\0';
                printf("connected\nquery %s\n", buffer);
                
                n = 0;
                if(buffer[0] == 'q')
                {
                    if((reqid = query_word(&buffer[2])) > 0)
                    {

                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer, "s %d", reqid);
                        n = 1 + 4 + 4;
                    }
                    else
                    {
                        memset(buffer, 0, sizeof(buffer));
                        n = 1 + 15 +4;
                        sprintf(buffer, "f cant query word");
                    }
                }
            
                else if(buffer[0] == 'g')
                {
                    sscanf(&buffer[2], "%d", &reqid);
                    if((query_resp = check(reqid)).reqid > 0)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer, "s %s", query_resp.resptext);
                        n = 1 + strlen(query_resp.resptext) + 4;
                    }
                    else
                    {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer, "f unable to find meangin");
                        n = 1 + 23 +4;
                    }
                }
            
                if(n > 0)
                {
                    buffer[n] = '\0';
                    write(client_sockfd, buffer, n);
                }
                close(client_sockfd);
            }

        }
    }
    db_cleanup();
}
