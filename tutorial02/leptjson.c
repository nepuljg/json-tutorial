#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include<string.h>
#include<math.h>
#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

/* true , false , null*/ 
static int lept_parse_literal(lept_context*c , lept_value* v)
{
     if(memcmp("true", c->json, 4) == 0)
     {
        c->json +=4;
        v->type = LEPT_TRUE;
        return LEPT_PARSE_OK;
     }else if(memcmp("false", c->json, 5) == 0)
     {
        c->json +=5;
        v->type = LEPT_FALSE;
        return LEPT_PARSE_OK;
     }else if(memcmp("null", c->json, 4) == 0)
     {
        c->json +=4;
        v->type = LEPT_NULL;
        return LEPT_PARSE_OK;
     }
     return LEPT_PARSE_INVALID_VALUE;
}
static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);
    if (c->json == end || end == NULL)
        return LEPT_PARSE_INVALID_VALUE;
    if(*c->json == '+') 
    {
        c->json++;
        return LEPT_PARSE_INVALID_VALUE;
    }
    if(*c->json == '.')
    {
        c->json++;
        return LEPT_PARSE_INVALID_VALUE;
    }
    if(*end != '\0')
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
    if(*(end-1) == '.')
    {
        return LEPT_PARSE_INVALID_VALUE;
    }
    if((memcmp("inf", c->json, 3) == 0) || (memcmp("INF", c->json, 3) ==0) || 
        (memcmp("NAN", c->json, 3) == 0) || (memcmp("nan", c->json, 3) == 0))
        {
            return LEPT_PARSE_INVALID_VALUE;
        }
    if(strlen(c->json) > 1 && (memcmp("0",c->json, 1) == 0 || memcmp("0x", c->json, 2) == 0 || memcmp("0X", c->json, 2) == 0))
    {
        return LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    
    if(isinf(v->n))
    {
        return LEPT_PARSE_NUMBER_TOO_BIG;
    }
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c, v);
        case 'f':  return lept_parse_literal(c, v);
        case 'n':  return lept_parse_literal(c, v);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
