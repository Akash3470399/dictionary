#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


int is_word(char *str)
{
	char ch;	
	int res = (*str)? 1 : 0;	
	while((*str != '\0') && (res == 1))
	{
		ch = *str;
		if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		{
			*str |= 0x60; 	
			str += 1;
		}
		else
			res = 0;
	}	
	return res;
}


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
	sscanf(req, "%c-%s", &op, query);	
	char buffer[1000];

	if(op == 'q' && is_word(query)) {}
	else if(op == 'g' && (atoi(query) > 0)) {}
	else
	{
		puts("Status: 400 Bad Request\n\r");
		return 0;
	}


	if(connect(server_sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == 0)
	{
        write(server_sockfd, req, strlen(req));
        if(read(server_sockfd, buffer, 100) > 0)
		{	
			puts("Status: 200 OK\n\r");
			printf("{\"resptext\": \"%s\"}\r", &buffer[2]);
			puts("\r");
		}
	}

	close(server_sockfd);
	return 0;
}
