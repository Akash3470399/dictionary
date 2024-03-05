#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    struct sockaddr_in serveraddr;
    int server_sockfd, server_port = 6666;

    if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket createion failed\n");
        return 1;
    }
    
    memset(&serveraddr, 0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);


    char buff[100];
    char rbuff[100];
    sprintf(buff, "e %d", 1);
    if(connect(server_sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == 0)
    {
        printf("connected\n");

        write(server_sockfd, buff, strlen(buff));

        int n = read(server_sockfd, buff, 100);
        buff[n] = '\0';
        printf("res %s\n", buff);
        close(server_sockfd);
    }
    else
        printf("err\n");



    printf("done\n");
    close(server_sockfd);
    return 0;
}
