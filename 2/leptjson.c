#include "leptjson.h"
#include <assert.h>    /* assert() */
#include <errno.h>    /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */
#include <stdlib.h>    /* NULL, strtod() */

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++;} while(0)
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
} lept_context;

/* ws = *(%x20/%x09/%x0A/%x0D) */
static void lept_parse_whitespace(lept_context* c)
{
    const char* p = c->json;
    while (*p == ' '|| *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

// deprecated use lept_parse_literal(c,v,literal, type)
static int lept_parse_true(lept_context* c, lept_value* v)
{
    EXPECT(c, 't');
    if(c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

// deprecated use lept_parse_literal(c,v,literal, type)
static int lept_parse_false(lept_context* c, lept_value* v)
{
    EXPECT(c, 'f');
    if(c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

// deprecated use lept_parse_literal(c,v,literal, type)
static int lept_parse_null(lept_context* c, lept_value* v)
{
    EXPECT(c, 'n');
    if(c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

/* parse null/true/false in one function */
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type)
{
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i+1]; i++) {
        if(c->json[i] != literal[i+1])
            return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

// static int lept_parse_number(lept_context* c, lept_value* v)
// {
//     char* end;
//     v->n = strtod(c->json, &end);
//     if(c->json == end)
//         return LEPT_PARSE_INVALID_VALUE;
//     c->json = end;
//     v->type = LEPT_NUMBER;
//     return LEPT_PARSE_OK;
// }

static int lept_parse_number(lept_context* c, lept_value* v)
{
    const char* p = c->json;
    if(*p == '-') ++p;
    if(*p == '0') ++p;
    else {
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        // for(p++; ISDIGIT(*P); p++);
        while(ISDIGIT(*p)) p++;
    }
    if(*p == '.') {
        ++p;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        // for(p++; ISDIGIT(*P); p++);
        while(ISDIGIT(*p)) p++;
    }
    if(*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') ++p;
        if(!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        // for(p++;ISDIGIT(*p); p++);
        while(ISDIGIT(*p)) p++;
    }
    errno = 0;
    v->n = strtod(c->json, NULL);
    if(errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = p;   // *p = '\0'
    // if no c->json = p, in function lept_parse(v, json)
    // if(*c->json != NULL) will return v->type = LEPT_NULL and LEPT_PARSE_ROOT_NOT_SINGULAR
    return LEPT_PARSE_OK;
    

}

/* value = null/true/false/number */
static int lept_parse_value(lept_context* c, lept_value* v)
{
    switch(*c->json) {
        // case 't': return lept_parse_true(c, v);
        // case 'f': return lept_parse_false(c,v);
        // case 'n': return lept_parse_null(c, v);
        case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
        case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
        default: return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
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
    if((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if(*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v)
{
    assert(v != NULL);
    return v->type;
}

/* only when type == LEPT_NUMBER, n represent number in JSON */
double lept_get_number(const lept_value* v)
{
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}