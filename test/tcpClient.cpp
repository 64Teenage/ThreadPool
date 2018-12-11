#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


#define SERV_PORT       8000
#define LISTENQ         1024
#define MAXLINE         1024


int main(int argc, char ** argv)
{
    int     sockfd;
    struct sockaddr_in  servaddr;
    char    sendline[MAXLINE], recvline[MAXLINE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[2], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    strcpy(sendline, argv[1]);
    int nSize = strlen(sendline);
    while (nSize>0)
    {
        int iSuccess = write(sockfd, sendline, nSize);
        nSize -= iSuccess;
    }

    while ((nSize=read(sockfd, recvline, MAXLINE))>0)
        fputs(recvline, stdout);

}