{
    "name":"runoob",
    "alexa":10000,
    "shopping":[ "jingdong", "pingduoduo", "Taobao" ]
    "sites": {
        "site1":"www.runoob.com",
        "site2":"m.runoob.com",
    }  
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <json-c/json_object.h>

/*
typedef enum json_type 
{
    json_type_null,
    json_type_boolean,
    json_type_double,
    json_type_int,
    json_type_object,
    json_type_array,
    json_type_string,
} json_type;
*/


/* 创建一个object类型的Jason对象，返回地址 */
struct json_object* json_object_new_object(void);


/* 
    往json对象中添加键值对
    key为C字符串
    value为C转换后对应的Jason类型对象
 */
void json_object_object_add(struct json_object *obj, const char*key, struct json_object *value);


/* 删除键为"name"的键值对 */
void json_object_object_del(struct json_object *obj, const char*key);

/* 获取Jason对象中指定键的值 */
json_object *json_object_object_get(json_object *obj, const char *key);

/* 创建一个整数类型的Jason类型对象，并返回地址，i为C类型的整数 */
struct json_object* json_object_new_int(int32_t i);

/* 获取整数型Jason类型对象的C整数，以返回值返回 */
int32_t json_object_get_int(struct json_object *obj);

/* 创建一个字符串类型的Jason类型对象， 并返回地址，s为C类型的字符串 */
struct json_object* json_object_new_string(const char *s);

/* 获取字符串类型Jason类型对象的C字符串，以返回值返回 */
const char *json_object_get_string(struct json_object *obj);

/* 创建一个数组类型的Jason类型对象，返回地址 */
struct json_object* json_object_new_array(void);

/* 将Jason类型对象val添加到Jason数组类型对象obj中 */
int json_object_array_add(struct json_object *obj, struct json_object *val);

/* 返回JSON对象的数组值。 */
struct array_list *json_object_get_array(json_object *obj);

/* 返回JSON数组中的元素个数 */
int json_object_array_length(json_object *array);

/* 将符合json格式的str字符串构造成一个json对象。 */
struct json_object* json_tokener_parse(const char *str);

/* 将json对象内容，转成json格式对应的的C字符串格式。 */
const char* json_object_to_json_string(struct json_object *obj);

/* 将JSON对象写入文件中。 */
int json_object_to_file(char *filename, json_object *obj);

/* 从文件中读取JSON数据并返回一个JSON对象。 */
json_object *json_object_from_file(char *filename);

/* 获取JSON对象的类型。 */
enum json_type json_object_get_type(struct json_object *obj);

/* 释放JSON对象 */
void json_object_put(struct json_object *obj);

