#ifndef LEPTJSON_H_
#define LEPTJSON_H_

/**
* JSON中的6中数据类型，把true和false当作两种类型就是7种
* 声明一个枚举类型，包含了JSON的7中数据类型
*/
typedef enum {
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
} lept_type;

/**
    * 声明JSON的数据结构。
    * 实现一个树的数据结构，每个节点用lept_value结构体表示，将其称为一个JSON值
    */
typedef struct
{
    lept_type type;
} lept_value;

/**
    * lept_parse返回的值为一下枚举值
    * 无错误会返回LEPT_PARSE_OK
    */
    enum
    {
        LEPT_PARSE_OK = 0,
        LEPT_PARSE_EXPECT_VALUE,
        LEPT_PARSE_INVALID_VALUE,
        LEPT_PARSE_ROOT_NOT_SINGULAR
    };

/**
    * 解析JSON的API函数
    * 传入的JSON文本是一个C字符串(空结尾字符串)
    */
int lept_parse(lept_value *v, const char *json);

/**
    * 访问结果函数，获取其类型
    */
lept_type lept_get_type(const lept_value *v);

#endif