#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* 位域的使用: 目的: 减少带宽 - 资源 */
struct Person
{
    unsigned char age; 
    char sex:1;     /* 0:男士 1:女生 */
    char weight:7;  /* */
};

int main()
{
    const char * str = "7f7543257a415bbe301ba91622fa7c19e93a6635";

    int length = strlen(str);

    printf("length:%d\n", length);

    return 0;
}