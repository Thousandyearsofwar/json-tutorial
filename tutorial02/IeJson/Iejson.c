#include "Iejson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* strtod(c->json, &end); */

#include <math.h>
#include <errno.h>

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

static int ie_parse_literal(ie_context* c,
							const char* checkLiteral,
							ie_type type,
							ie_value* v)
{
	/* 输入是外部数据，不能只用 assert 校验 */
	if (*c->json != checkLiteral[0])
		return IE_PARSE_INVALID_VALUE;

	c->json++;

	//首字符后续的检测
	size_t i;
	for (i = 0; checkLiteral[i + 1] != '\0'; i++)
	{
		if (c->json[i] != checkLiteral[i + 1])
			return IE_PARSE_INVALID_VALUE;
	}
	c->json += i;//json指针偏移回到'\0'

	v->type = type;
	return IE_PARSE_OK;
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
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')


static int ie_parse_number(ie_context* c, ie_value* v)
{
	/* JSON number grammar (RFC 8259):
	 *   number = [ "-" ] int [ frac ] [ exp ]
	 *   int    = "0" / ( digit1-9 *DIGIT )
	 *   frac   = "." 1*DIGIT
	 *   exp    = ( "e" / "E" ) [ "-" / "+" ] 1*DIGIT
	 * strtod() is too permissive ("1.", ".5", "INF", "nan", "0x1p3" all parse),
	 * so validate with p first and only then hand the text to strtod().
	 */
	const char* p = c->json;

	//处理整数部分
	if (*p == '-')
		p++;							/* '-' may only appear here */
	if (*p == '0')
	{
		p++;							/* "0" must not be followed by a digit */
	}
	else
	{
		//负数符号位后开头
		if (!ISDIGIT1TO9(*p))
			return IE_PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);    //-1 -2 -3 ....跑到下一个非数字字符
	}

	//处理小数部分
	if (*p == '.') {                    /* at least one digit after '.' */
		p++;
		if (!ISDIGIT(*p))
			return IE_PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}

	//科学计数法部分 e/E开头 +/- 数字
	if (*p == 'e' || *p == 'E') {       /* at least one digit in the exponent */
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!ISDIGIT(*p))				//+/- 后续如果没有数字即为非法
			return IE_PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}

	errno = 0;
	v->numberValue = strtod(c->json, NULL);
	if (errno == ERANGE && (v->numberValue == HUGE_VAL || v->numberValue == -HUGE_VAL))
		return IE_PARSE_NUMBER_TOO_BIG;   /* ✓ 只有上溢才算错 */
	v->type = IE_NUMBER;
	c->json = p;                        /* p, not strtod's end: keeps leftovers */
	return IE_PARSE_OK;
}

static int ie_parse_value(ie_context* c, ie_value* v) {
	switch (*c->json) {
		case 't':  return ie_parse_literal(c, "true", IE_TRUE, v);
		case 'f':  return ie_parse_literal(c, "false", IE_FALSE, v);
		case 'n':  return ie_parse_literal(c, "null", IE_NULL, v);
		default:   return ie_parse_number(c, v);
		case '\0': return IE_PARSE_EXPECT_VALUE;

	}
}
int ie_parse(ie_value* value, const char* jsonRawData)
{
	ie_context c = { jsonRawData };
	assert(value != NULL);
	value->type = IE_NULL;
	ie_parse_whitespace(&c);

	int ret = ie_parse_value(&c, value);
	//null x => nullx 只处理到null就结束了，后面还有字符即判定为NOT_SINGULAR
	//0123，0x0，0x123 只处理到0就结束了
	if (ret == IE_PARSE_OK) {
		ie_parse_whitespace(&c);
		if (*c.json != '\0') {
			value->type = IE_NULL;
			ret = IE_PARSE_ROOT_NOT_SINGULAR;
		}
	}

	return ret;
}

ie_type ie_get_type(const ie_value* value)
{
	assert(value != NULL);
	return value->type;
}

double ie_get_number(const ie_value* value)
{
	assert(value != NULL && value->type == IE_NUMBER);
	return value->numberValue;
}
