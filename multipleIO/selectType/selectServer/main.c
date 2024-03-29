#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/select.h>

#define SERVER_PORT 8080
#define  BUFFER_SIZE 64

enum FUNCTION_OPTION
{
    REGIS = 1,
    LOGIN,
};

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }
    printf("sockfd is %d\n", sockfd);


    /* 设置端口复用 */
    int optVal = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optVal, sizeof(optVal));
    if (ret == -1)
    {
        perror("setsockopt error");
        exit(-1);
    }

    struct sockaddr_in localAddress;
    /* 清除脏数据 */
    memset(&localAddress, 0, sizeof(localAddress));
    localAddress.sin_family = AF_INET;      // ipv4
    localAddress.sin_port = htons(SERVER_PORT);
    localAddress.sin_addr.s_addr = INADDR_ANY;

    socklen_t localAddressLength = sizeof(localAddress);
    /* 绑定端口IP. */
    ret = bind(sockfd, (const struct sockaddr *)&localAddress, localAddressLength);
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

    /* 
        0: 标准输入
        1: 标准输出
        2: 标准出错 
    */
    int maxfd = sockfd;

    fd_set readfds;
    /* 清除读集合 */
    FD_ZERO(&readfds);
    /* 将sockfd放到读集合中, 想让内核帮我去检测该fd是否可读 */
    FD_SET(sockfd, &readfds);


    /* 客户端的信息 */
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t clientAddressLength = 0;


    /* 备份 */
    fd_set tmpReadfds;
    FD_ZERO(&tmpReadfds);

    while (1)
    {
        /* tmpReadfds集合表示就绪的事件 */
        tmpReadfds = readfds;

        /* 监听读集合是否有读事件 */
        int res = select(maxfd + 1, &tmpReadfds, NULL, NULL, NULL);
        if (res == -1)
        {
            perror("select error");
            exit(-1);
        }
        /* 程序到这个地方, tmpReadfds有当前可读的事件集合 */

        /* 判断集合里面是否包含sockfd */
        if (FD_ISSET(sockfd, &tmpReadfds))
        {
            /* 处理连接 */
            int connfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLength);
            if (connfd == -1)
            {
                perror("accept error");
                exit(-1);
            }
            printf("accept ok!\n");
            /* 下面就应该让connfd加入到readfds集合中 */
            FD_SET(connfd, &readfds);
            /* 最大文件描述符加一 */
            maxfd = maxfd > connfd ? maxfd : connfd;
        }

        /* 程序到这个地方 说明有人通信(点菜) */
        for (int idx = 4; idx < maxfd + 1; idx++)
        {
            if (FD_ISSET(idx, &tmpReadfds))
            {
                /* 程度到这边 idx其实就是通信句柄 */
                int connfd = idx;

                /* 服务器接收客户端的信息, 并做出反馈 */
                /* 服务器365天 24小时 */
                char readBuffer[BUFFER_SIZE] = { 0 };
                int readBytes = 0;

                char writeBuffer[BUFFER_SIZE] = { 0 };
                int writeBytes = 0;
                
                readBytes = read(connfd, readBuffer, BUFFER_SIZE - 1);
                if (readBytes < 0)
                {
                    /* 读出错 */
                    printf("read error");
                    FD_CLR(connfd, &readfds);
                }
                else if (readBytes == 0)
                {
                    /* 这边读到的字节数为0, 说明什么? */
                    printf("客户端下线\n");
                    FD_CLR(connfd, &readfds);
                }
                else
                {
                    printf("readBytes:%d,\treadBuffer:%s\n", readBytes, readBuffer);

                    for (int jdx = 0; jdx < readBytes; jdx++)
                    {
                        writeBuffer[jdx] = toupper(readBuffer[jdx]);
                    }

                    write(connfd, writeBuffer, readBytes);
                }
            }
        }

    }

#if 0
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t clientAddressLength = 0;
    
    /* 单进程并发 */
    /* 多进程 */
    /* 多线程 */
    /* 单进程 - [redis]-内存数据库 减少IO 
        1. select
            1. 跨平台 mac, linux, windows
            2. 底层:动态数组
            3. monitor  only file descriptors numbers that are less than FD_SETSIZE.  
        2. epoll    
            1. 只适用于linux
            2. 底层:红黑树
            3.
    */

    /* accept的返回值是和【客户端通信】的文件描述符 */
    int connfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (connfd == -1)
    {
        perror("accept error");
        exit(-1);
    }
    printf("connfd is %d\n", connfd);


    

    /* 回收资源 */
    close(connfd);
#endif
    close(sockfd);
    return 0;
}