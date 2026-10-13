#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Iejson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

static void test_parse_null() {
	/* Deliberately start with another type: ie_parse must overwrite it. */
	ie_value v = { IE_TRUE };
	EXPECT_EQ_INT(IE_PARSE_OK, ie_parse(&v, "null"));
	EXPECT_EQ_INT(IE_NULL, ie_get_type(&v));
}

static void test_parse_true() {
	ie_value v = { IE_NULL };
	EXPECT_EQ_INT(IE_PARSE_OK, ie_parse(&v, "true"));
	EXPECT_EQ_INT(IE_TRUE, ie_get_type(&v));
}

static void test_parse_false() {
	ie_value v = { IE_NULL };
	EXPECT_EQ_INT(IE_PARSE_OK, ie_parse(&v, "false"));
	EXPECT_EQ_INT(IE_FALSE, ie_get_type(&v));
}

static void test_parse_invalid_value() {
	ie_value v = { IE_NULL };
	/* Only lowercase literals are valid: uppercase and mixed case are rejected. */
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "TRUE"));
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "True"));
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "truE"));
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "FALSE"));
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "False"));
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "falsE"));
	/* Incomplete or unrelated literals. */
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "nul"));
	EXPECT_EQ_INT(IE_PARSE_INVALID_VALUE, ie_parse(&v, "?"));
}

static void test_parse() {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_invalid_value();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}