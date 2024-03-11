#include <stdio.h>
#include <json-c/json.h>


/* 
    需求: 将{ "type": 2, "name":"zhou", "passwd" : 123 }
*/

/* json是<key, value> */
int main()
{
    /* step1: 创建json对象 */
    struct json_object * object = json_object_new_object();
    if (object == NULL)
    {
        perror("json_object_new_object error");
        exit(-1);
    }

    /* 在json对象中添加<key, value> */
    json_object_object_add(object, "type", json_object_new_int64(1));
    json_object_object_add(object, "name", json_object_new_string("zhou"));
    json_object_object_add(object, "passwd", json_object_new_int64(123));
    
    /* 将json对象转化成字符串 */
    const char * str = json_object_to_json_string(object);
    printf("str:%s\n", str);

    /* ===================================================== */
    /* 将jons类型的字符串解析出来 */
    /* 1. 将json类型的字符串解析成json对象 */
    struct json_object * obj = json_tokener_parse(str);

    printf("type:%ld\n", json_object_get_int64(json_object_object_get(obj, "type")));
    printf("name:%s\n", json_object_get_string(json_object_object_get(obj, "name")));
    printf("passwd:%ld\n", json_object_get_int64(json_object_object_get(obj, "passwd")));

    return 0;
}