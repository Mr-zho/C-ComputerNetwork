#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <json-c/json.h>

#define SERVER_PORT 8080
#define  BUFFER_SIZE 64

#define USERINFO_SIZE   16

enum FUNCTION_OPTION
{
    REGIS = 1,
    LOGIN,
    PRI_CHAT,
    GROUP_CHAT,
};

/* 结构体 */
typedef struct UserInfo 
{
    enum FUNCTION_OPTION type;
    char username[USERINFO_SIZE];
    char passwd[USERINFO_SIZE];
} UserInfo;
/* 小包传输 */

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

    /* 方式1: 最差劲👎的方式是 */
    #if 0
    UserInfo info;
    memset(&info, 0, sizeof(info));
    #endif
    while (1)
    {
        #if 1
        readBytes = read(connfd, &readBuffer, sizeof(readBuffer) - 1);
        #else
        readBytes = read(connfd, (void *)&info, sizeof(info));
        #endif
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
            #if 1
            printf("readBytes:%d,\t readBuffer:%s\n", readBytes, readBuffer);
            #else
            printf("readBytes:%d,\t type:%d,\tname:%s,\tpasswd:%s\n", readBytes, info.type, info.username, info.passwd);
            #endif

            enum FUNCTION_OPTION choice;
            #if 0
            /* 方式2: 内存拷贝 */
            memcpy((void *)&choice, readBuffer, sizeof(choice));
            printf("choice:%d\n", choice);
            #else
            /* json传递 */
            struct json_object * obj = json_tokener_parse(readBuffer);
            choice = json_object_get_int64(json_object_object_get(obj, "type"));
            printf("choice:%d\n", choice);
            #endif
            
            switch (choice)
            {
            case REGIS:
                {
                    #if 0
                    UserInfo *info = (UserInfo *)readBuffer;
                    printf("username:%s, info->passwd:%s\n", info->username, info->passwd);
                    #else
                    printf("name:%s\n", json_object_get_string(json_object_object_get(obj, "name")));
                    printf("passwd:%ld\n", json_object_get_int64(json_object_object_get(obj, "passwd")));
                    #endif
                }
                break;
            case LOGIN:
                {
                    #if 0
                    UserInfo *info = (UserInfo *)readBuffer;
                    printf("username:%s, info->passwd:%s\n", info->username, info->passwd);
                    #else
                    printf("name:%s\n", json_object_get_string(json_object_object_get(obj, "name")));
                    printf("passwd:%ld\n", json_object_get_int64(json_object_object_get(obj, "passwd")));
                    #endif
                }
                break;
            case PRI_CHAT:
                break;
            case GROUP_CHAT:
                break;
            
            default:
                break;
            }
            // for (int idx = 0; idx < readBytes; idx++)
            // {
            //     /* 转大写 */
            //     writeBuffer[idx] = toupper(readBuffer[idx]);
            // }

            // /* 写回客户端 */
            // write(connfd, writeBuffer, readBytes);
        }
    }

    /* 回收资源 */
    close(connfd);
    close(sockfd);
    return 0;
}