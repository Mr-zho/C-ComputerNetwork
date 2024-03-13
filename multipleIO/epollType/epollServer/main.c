#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <stdbool.h>

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

    /* 创建一个epoll instance */
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll create error");
        exit(-1);
    }


    struct epoll_event event;
    memset(&event, 0, sizeof(event));

    event.data.fd = sockfd;
    event.events = EPOLLIN;
    /* 将sockfd 挂载到epoll instance中 */
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);



    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t clientAddressLength = 0;
    
    struct epoll_event events[BUFFER_SIZE];
    /* 清空脏数据 */
    memset(events, 0, sizeof(struct epoll_event) * BUFFER_SIZE);
    
    int num = 0;
    while (true)
    {
        num = epoll_wait(epfd, events, BUFFER_SIZE, -1);
        if (num == -1)
        {
            perror("epoll wait...");
            exit(-1);
        }
        /* 程序到这个地方 说明什么? */
        /* 说明: 一定有人跟我连接 */
        printf("num:%d\n", num);

        for (int idx = 0; idx < num; idx++)
        {
            /* 现在的idx其实是 文件描述符 */
            int fd = events[idx].data.fd;
            if (fd == sockfd)
            {
                /* accept的返回值是和【客户端通信】的文件描述符 */
                int connfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLength);
                if (connfd == -1)
                {
                    perror("accept error");
                    exit(-1);
                }
                printf("connfd is %d\n", connfd);

                struct epoll_event connevent;
                memset(&connevent, 0, sizeof(connevent));

                connevent.data.fd = connfd;
                connevent.events = EPOLLIN;
                /* 把connfd加入到epoll instance. */
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &connevent);
            }
            else
            {
                char readBuffer[BUFFER_SIZE] = { 0 };
                int readBytes = 0;

                char writeBuffer[BUFFER_SIZE] = { 0 };
                int writeBytes = 0;
                
                readBytes = read(fd, readBuffer, BUFFER_SIZE - 1);
                if (readBytes < 0)
                {
                    /* 读出错 */
                    perror("read error");
                    /* 将该文件描述符从epoll instance 删除掉. */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    /* 关闭文件描述符 */
                    close(fd);
                }
                else if (readBytes == 0)
                {
                    /* 这边读到的字节数为0, 说明什么? */
                    printf("客户端下线\n");
                    /* 将该文件描述符从epoll instance 删除掉. */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    /* 关闭文件描述符 */
                    close(fd);
                }
                else
                {
                    printf("readBytes:%d,\treadBuffer:%s\n", readBytes, readBuffer);

                    for (int jdx = 0; jdx < readBytes; jdx++)
                    {
                        writeBuffer[jdx] = toupper(readBuffer[jdx]);
                    }

                    write(fd, writeBuffer, readBytes);
                }
                
            }
        }
    }


    close(sockfd);
    return 0;
}