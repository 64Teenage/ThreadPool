#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "threadpool.h"

#define SERV_PORT       9877
#define LISTENQ         1024
#define MAXLINE         1024

void    server(int  fd);
ssize_t						/* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n);

int main(int argc, char ** argv)
{
    int     listenfd, connfd, sockfd;
    int     client[FD_SETSIZE];
    struct sockaddr_in  servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        printf("create socket error : %s\n", strerror(errno));
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    int iState = bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
    if (iState == -1)
    {
        printf("bind error : %s\n", strerror(errno));
        exit(0);
    }

    iState = listen(listenfd, LISTENQ);
    if (iState == -1)
    {
        printf("listen error : %s\n", strerror(errno));
        exit(0);
    }


    //ThreadPool pool(4);
    //pool.init();

    while (true)
    {
        socklen_t   clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd==-1)
        {
            printf("accept error : %s\n",strerror(errno));
            exit(0);
        }

        //pool.submit(server, connfd);
        server(connfd);
    }
}

void    server(int sockfd)
{
    char    buf[MAXLINE],sendline[MAXLINE];
    int     count=0, n=0;


    while ((count=read(sockfd, buf, MAXLINE))>0)
    {
        int fd = open(buf, O_RDONLY | O_EXCL);
        if (fd == -1)
        {

            strcpy(sendline, strerror(errno));
            int wholeLen = strlen(sendline);

            sendline[wholeLen] = '\0';
            while (wholeLen>0)
            {
                int iSuccess = write(sockfd, sendline, wholeLen);
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
    }
    //buf[count] = '\0';

    close(sockfd);
}

ssize_t						/* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
    size_t	nleft;
    ssize_t	nread;
    char	*ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;		/* and call read() again */
            else
                return(-1);
        } else if (nread == 0)
            break;				/* EOF */

        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);		/* return >= 0 */
}
