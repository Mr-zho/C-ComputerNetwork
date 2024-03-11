#include <stdio.h>
#include <json-c/json.h>


/* 
    需求: 将
    { "type": 2, "name":"zhou", "passwd" : 123 , "books" : ["CPP", "JAVA", "PHP"] }
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

    struct json_object * arrayBooks = json_object_new_array();
    json_object_array_add(arrayBooks, json_object_new_string("C++"));
    json_object_array_add(arrayBooks, json_object_new_string("JAVA"));
    json_object_array_add(arrayBooks, json_object_new_string("PHP"));
    json_object_object_add(object, "books", arrayBooks);
    
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

    struct json_object * bookLists = json_object_object_get(obj, "books");
    /* 数组有多长 */
    size_t size = json_object_array_length(bookLists);
    printf("size:%ld\n", size);
    for (int idx = 0; idx < size; idx++)
    {
        printf("book[%d] = %s\n", idx, json_object_get_string(json_object_array_get_idx(bookLists, idx)));
    }
    return 0;
}