#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main() 
{
	struct sockaddr_in serveraddr;
    int server_sockfd, server_port = 6666;
	char op, query[100];

    if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //printf("socket createion failed\n");
        return 1;
    }
    
    memset(&serveraddr, 0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    char *req = getenv("QUERY_STRING");
	sscanf(req, "%c=%s", &op, query);	
	char buffer[1000];

	if(op == 'q' && strlen(query) > 0)
		req[1] = ' ';				
	else if(op == 'g' && (atoi(query) > 0))
		req[1] = ' ';
	else
	{
		puts("Status: 400 Bad Request\n\r");
		return 0;
	}


	if(connect(server_sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == 0)
	{

        write(server_sockfd, req, strlen(req));
        int n = read(server_sockfd, buffer, 100);
	
		puts("Status: 200 OK\n\r");
		printf("{\"resptext\": \"%s\"}\r", buffer);
		puts("\r");
	}

	close(server_sockfd);
	return 0;
}
