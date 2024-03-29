#include <sys/types.h>         
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <json-c/json.h>

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


/* 映射: 全局指针数组做映射todo..., */
#if 0
static const char * convertJsonStr(const UserInfo * info, int keyNums, const char *keys[])
{
    /* step1: 创建json对象 */
    struct json_object * object = json_object_new_object();
    if (object == NULL)
    {
        perror("json_object_new_object error");
        exit(-1);
    }

    /* 在json对象中添加<key, value> */
    for (int idx = 0; idx < keyNums; idx++)
    {
        json_object_object_add(object, keys[idx], json_object_new_int64(info->type));
    }
    json_object_object_add(object, key2, json_object_new_string(info->username));
    json_object_object_add(object, key3, json_object_new_string(info->passwd));


    /* 将json对象转化成字符串 */
    const char * str = json_object_to_json_string(object);
    printf("str:%s\n", str);
}
#endif

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
        #if 1
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
        #elif 0
        
        /* step1: 创建json对象 */
        struct json_object * object = json_object_new_object();
        if (object == NULL)
        {
            perror("json_object_new_object error");
            exit(-1);
        }

        int type;
        printf("please input type:");
        scanf("%d", (int *)&type);
        char nameBuffer[USERINFO_SIZE] = { 0 };
        printf("please input name:");
        scanf("%s", nameBuffer);
        char passwdBuffer[USERINFO_SIZE] = { 0 };
        printf("please input passwd:");
        scanf("%s", passwdBuffer);


        /* 在json对象中添加<key, value> */
        json_object_object_add(object, "type", json_object_new_int64(type));
        json_object_object_add(object, "name", json_object_new_string(nameBuffer));
        json_object_object_add(object, "passwd", json_object_new_string(passwdBuffer));
        
        /* 将json对象转化成字符串 */
        const char * str = json_object_to_json_string(object);
        printf("str:%s\n", str);
        #else
        /* 注册功能 */
        printf("please input type:");
        scanf("%d", (int *)&info.type);
        printf("please input name:");
        scanf("%s", info.username);
        printf("please input passwd:");
        scanf("%s", info.passwd);

        
        /* step1: 创建json对象 */
        struct json_object * object = json_object_new_object();
        if (object == NULL)
        {
            perror("json_object_new_object error");
            exit(-1);
        }


        json_object_object_add(object, "type", json_object_new_string(info.type));
        json_object_object_add(object, "name", json_object_new_string(info.username));
        json_object_object_add(object, "passwd", json_object_new_string(info.passwd));

        /* 将json对象转化成字符串 */
        const char * str = json_object_to_json_string(object);
        printf("str:%s\n", str);

        /* 写到服务器 */
        writeBytes = write(sockfd, str, strlen(str) + 1);
        if (writeBytes < 0)
        {
            perror("write error");
            break;
        }
        #endif


        
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