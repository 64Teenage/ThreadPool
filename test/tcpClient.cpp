#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


#define SERV_PORT       9877
#define LISTENQ         1024
#define MAXLINE         1024

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n);

int main(int argc, char ** argv)
{
    int     sockfd;
    struct sockaddr_in  servaddr;
    char    sendline[MAXLINE], recvline[MAXLINE];

    memset(recvline, 0, MAXLINE*sizeof(char));
    memset(sendline, 0, MAXLINE*sizeof(char));

    /*
    if (argc != 2)
    {
        printf("usage : tcpClient <ip>\n");
        exit(0);
    }
    */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    //inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    char * strIP = "127.0.0.1";
    inet_pton(AF_INET, strIP, &servaddr.sin_addr);

    int iState = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (iState==-1)
    {
        printf("connect error : %s.\n", strerror(errno));
        exit(0);
    }

    FILE * fp = stdin;
    while ((fgets(sendline, MAXLINE, fp))!=NULL)
    {
        int nSize = strlen(sendline);
        if (sendline[nSize-1] == '\n')
            --nSize;
        writen(sockfd, sendline, nSize);

        while ((nSize=read(sockfd, recvline, MAXLINE))>0)
        {
            printf("%s", recvline);
            memset(recvline, 0, MAXLINE*sizeof(char));
        }
    }


}

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t		nleft;
    ssize_t		nwritten;
    const char	*ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;		/* and call write() again */
            else
                return(-1);			/* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}
