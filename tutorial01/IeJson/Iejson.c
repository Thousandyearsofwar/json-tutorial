#include "Iejson.h"
#include <assert.h>  /* assert() */

#define EXPECT(c, ch)\
	do\
	{\
		assert(*c->json == (ch));\
		c->json++; \
	}\
	while(0)

typedef struct {
	const char* json;
}ie_context;

static void ie_parse_whitespace(ie_context* c) {
	const char* p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		p++;
	c->json = p;
}
static int ie_parse_null(ie_context* c, ie_value* v) {
	EXPECT(c, 'n');
	if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
		return IE_PARSE_INVALID_VALUE;
	c->json += 3;
	v->type = IE_NULL;
	return IE_PARSE_OK;
}

static int ie_parse_false(ie_context* c, ie_value* v) {
	EXPECT(c, 'f');
	/* JSON only accepts the lowercase literal "false". */
	if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
		return IE_PARSE_INVALID_VALUE;
	c->json += 4;
	v->type = IE_FALSE;
	return IE_PARSE_OK;
}

static int ie_parse_true(ie_context* c, ie_value* v) {
	EXPECT(c, 't');
	/* JSON only accepts the lowercase literal "true". */
	if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
		return IE_PARSE_INVALID_VALUE;
	c->json += 3;
	v->type = IE_TRUE;
	return IE_PARSE_OK;
}


static int ie_parse_value(ie_context* c, ie_value* v) {
	switch (*c->json) {
		case 'n':  return ie_parse_null(c, v);
		case 't':  return ie_parse_true(c, v);
		case 'f':  return ie_parse_false(c, v);
		case '\0': return IE_PARSE_EXPECT_VALUE;
		default:   return IE_PARSE_INVALID_VALUE;
	}
}
int ie_parse(ie_value* value, const char* jsonRawData)
{
	ie_context c = { jsonRawData };
	assert(value != NULL);
	value->type = IE_NULL;
	ie_parse_whitespace(&c);
	return ie_parse_value(&c, value);
}

ie_type ie_get_type(const ie_value* value)
{
	assert(value != NULL);
	return value->type;
}
