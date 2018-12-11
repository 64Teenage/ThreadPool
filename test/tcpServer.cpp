#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "ThreadPool.h"

#define SERV_PORT       8000
#define LISTENQ         1024
#define MAXLINE         1024

void    server(int  fd);

int main(int argc, char ** argv)
{
    int     listenfd, connfd, sockfd;
    int     client[FD_SETSIZE];
    struct sockaddr_in  servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    ThreadPool pool(4);
    pool.init();

    while (true)
    {
        socklen_t   clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

        pool.submit(server, connfd);
    }
}

void    server(int sockfd)
{
    char    buf[MAXLINE];
    int     count=0, n=0;

    while ((n=read(sockfd, buf+count, MAXLINE)))
        count += n;
    buf[count] = '\0';

    int fd = open(buf, O_RDONLY | O_EXCL);
    if (fd == -1)
    {
        strcpy(buf, strerror(errno));
        int wholeLen = strlen(buf);
        while (wholeLen>0)
        {
            int iSuccess = write(sockfd, buf, wholeLen);
            wholeLen -= iSuccess;
        }
    }
    else
    {
        int count = 1;
        while ((count=read(fd, buf, MAXLINE))>0)
        {
            while (count>0)
            {
                int iSuccess = write(sockfd, buf, count);
                count -= iSuccess;
            }
        }
    }

    close(fd);
    close(sockfd);
}