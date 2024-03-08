#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define SERVER_PORT 8080

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }
    printf("sockfd is %d\n", sockfd);


    struct sockaddr_in localAddress;
    /* 清除脏数据 */
    memset(&localAddress, 0, sizeof(localAddress));
    localAddress.sin_family = AF_INET;      // ipv4
    localAddress.sin_port = htons(SERVER_PORT);
    localAddress.sin_addr.s_addr = INADDR_ANY;

    socklen_t localAddressLength = sizeof(localAddress);
    /* 绑定端口IP. */
    int ret = bind(sockfd, (const struct sockaddr *)&localAddress, localAddressLength);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听套接字 */
    ret = listen(sockfd, 10);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t clientAddressLength = 0;
    int connfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (connfd == -1)
    {
        perror("accept error");
        exit(-1);
    }
    printf("connfd is %d\n", connfd);
    return 0;
}