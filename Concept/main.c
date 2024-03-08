#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

/* 联合体的内存是公用的. */
union data
{
    /* data */
    int data;
    char ch;
};


int main()
{
    /* 将点分十进制的ip地址 转换为大端的网络字节序 */
    int num = 0;
    const char * str = "192.168.10.7";
    inet_pton(AF_INET, str, &num);
    printf("num:%d\n", num);


    /* 将大端的网络字节序 转换为小端的点分十进制ip地址 */
    char buffer[32] = { 0 };
    inet_ntop(AF_INET, (void *)&num, buffer, sizeof(buffer));
    printf("buffer:%s\n", buffer);


    return 0;
}