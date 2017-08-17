#include <stdlib.h>    /* NULL */
#include <assert.h>    /* assert() */
#include "leptjson.h"

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++;} while(0)


/**
* 为了减少解析函数之间传递多个参数，把数据都放进一个lept_context结构体
*/
typedef struct {
    const char* json;
}lept_context;

/**
* 解析函数
*/
/* ws = *(%x20/%x09/%x0A/%x0D) */
static void lept_parse_whitespace(lept_context* c)
{
    const char* p = c->json;
    while(*p == ' '|| *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/* null = "null" */
static int lept_parse_null(lept_context* c, lept_value* v)
{
    EXPECT(c, 'n');
    if(c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

/* true = "true" */
static int lept_parse_true(lept_context* c, lept_value* v)
{
    EXPECT(c, 't');
    if(c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

/* false = "false" */
static int lept_parse_false(lept_context* c, lept_value* v)
{
    EXPECT(c, 'f');
    if(c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
        c->json += 4;
        v->type = LEPT_FALSE;
        return LEPT_PARSE_OK;
}

/* value = null/false/true */
static int lept_parse_value(lept_context* c, lept_value* v)
{
    switch(*c->json)
    {
        case 'n': return lept_parse_null(c,v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        case 't': return lept_parse_true(c,v);
        case 'f': return lept_parse_false(c, v);
        default: return LEPT_PARSE_INVALID_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json)
{
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK)
    {
        lept_parse_whitespace(&c);
        // 若 json 在一个值之后，空白之后还有其它字符，则要返回 LEPT_PARSE_ROOT_NOT_SINGULAR
        if(*c.json != '\0')    
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v)
{
    assert(v != NULL);
    return v->type;
}