#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 64

#define USERINFO_SIZE   16
#define MESSAGE_CONTENT 128

enum FUNCTION_OPTION
{
    REGIS = 1,
    LOGIN,
    PRI_CHAT,
    GROUP_CHAT,
};

/* 用户信息结构体 */
typedef struct UserInfo 
{
    enum FUNCTION_OPTION type;
    char username[USERINFO_SIZE];
    char passwd[USERINFO_SIZE];
} UserInfo;
/* 小包传输 */

typedef struct MessageInfo
{
    enum FUNCTION_OPTION type;
    char srcName[USERINFO_SIZE];
    char srcContent[MESSAGE_CONTENT];
    char dstName[USERINFO_SIZE];
} MessageInfo;



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
    const char * ip = "172.25.23.103";
    inet_pton(AF_INET, ip, &(localAddress.sin_addr.s_addr));
    socklen_t localAddressLength = sizeof(localAddress);
    #if 0
    // /* 绑定端口IP. */
    // int ret = bind(sockfd, (const struct sockaddr *)&localAddress, localAddressLength);
    // if (ret == -1)
    // {
    //     perror("bind error");
    //     exit(-1);
    // }
    #endif

    int ret = connect(sockfd, (const struct sockaddr *)&localAddress, localAddressLength);
    if (ret == -1)
    {
        perror("connect error");
        exit(-1);
    }

    printf("hello world");

    char writeBuffer[] = "hello nihao!";

    char readBuffer[BUFFER_SIZE] = { 0 };
    int readBytes = 0;

    UserInfo info;
    /* 清空脏数据 */
    memset(&info, 0, sizeof(info));

    int writeBytes = 0;
    while (1)
    {
        /* 注册功能 */
        printf("please input type:");
        scanf("%d", (int *)&info.type);
        printf("please input name:");
        scanf("%s", info.username);
        printf("please input passwd:");
        scanf("%s", info.passwd);

        /* 写到服务器 */
        writeBytes = write(sockfd, (const void *)&info, sizeof(info));
        if (writeBytes < 0)
        {
            perror("write error");
            break;
        }
        /* 写到服务器一定没有错 */

        sleep(2);

        // /* 读取 服务器发回的数据 */
        // readBytes = read(sockfd, readBuffer, BUFFER_SIZE);
        // if (readBytes < 0)
        // {
    
        // }
        // else if (readBytes == 0)
        // {

        // }
        // else
        // {
        //     printf("buffer:%s\n", readBuffer);
        // }
    }
    
    #if 0
    char buffer[32] = { 0 };
    int readBytes = read(sockfd, buffer, sizeof(buffer));
    if (readBytes < 0)
    {

    }
    else if (readBytes == 0)
    {

    }
    else
    {
        printf("buffer:%s\n", buffer);
    }
    #endif
    
    return 0;
}