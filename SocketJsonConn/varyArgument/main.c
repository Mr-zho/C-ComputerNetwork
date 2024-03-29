#include <stdio.h>

/* 可变参数... */
void test(const char * str, ...)
{
    printf("str:%s\n", str);
}

int main()
{
    // printf()
    test("123", "567");
    return 0;
}