#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    struct sockaddr_in selfaddr, clientaddr;
    int self_sockfd, port = 6666, client_sockfd, client_addrlen = 0;

    if((self_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket createion failed\n");
        return 1;
    }
    
    memset(&selfaddr, 0, sizeof(selfaddr));
    memset(&clientaddr, 0,sizeof(clientaddr));


    selfaddr.sin_family = AF_INET;
    selfaddr.sin_port = htons(port);
    selfaddr.sin_addr.s_addr = htons(INADDR_ANY);

    char buff[1000];

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

    //if((client_sockfd = accept(self_sockfd, (struct sockaddr*)&clientaddr, &client_addrlen) > 0)
    if((client_sockfd = accept(self_sockfd, NULL, NULL)) > 0)
    {

        printf("connected\n");
        int n = read(client_sockfd, buff, sizeof(buff));
        buff[n] = '\0';
        printf("data %s\n", buff);
        write(client_sockfd, buff, n);
        close(client_sockfd);
    }

    printf("done\n");
    close(self_sockfd);
    return 0;
}
