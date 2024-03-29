#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

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

    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t clientAddressLength = 0;
    
    /* accept的返回值是和【客户端通信】的文件描述符 */
    int connfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (connfd == -1)
    {
        perror("accept error");
        exit(-1);
    }
    printf("connfd is %d\n", connfd);

    /* 服务器接收客户端的信息, 并做出反馈 */
    /* 服务器365天 24小时 */
    char readBuffer[BUFFER_SIZE] = { 0 };
    int readBytes = 0;

    char writeBuffer[BUFFER_SIZE] = { 0 };
    int writeBytes = 0;
    while (1)
    {
        readBytes = read(connfd, readBuffer, BUFFER_SIZE - 1);
        if (readBytes < 0)
        {
            /* 读出错 */
            perror("read error");
            break;
        }
        else if (readBytes == 0)
        {
            /* 这边读到的字节数为0, 说明什么? */
            printf("客户端下线\n");
            break;
        }
        else
        {
            printf("readBytes:%d,\treadBuffer:%s\n", readBytes, readBuffer);

            /* 需要对readBuffer进行解析 */
            
            choice = XXXX("2zhou123456")
            switch (choice)
            {
            case REGIS:
                /* 改手机号已经被注册了 */
                /* 注册异常 */
                /* 注册成功 */
                write (connfd, "register error", )
                break;
            case LOGIN:
                /* */
                /* 密码错误 */
                /* 改用户不存在 */
                /* 账号被封 */
                /* 登陆成功 */
                write (connfd, "login error", )
                break; 

            default:
                break;
            }
        }
    }

    /* 回收资源 */
    close(connfd);
    close(sockfd);
    return 0;
}