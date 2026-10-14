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

#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define TEST_ERROR(parse_result , expect_type ,json)\
	do{\
		ie_value v;\
		v.type = IE_NULL;\
		EXPECT_EQ_INT(parse_result,ie_parse(&v,json));\
		EXPECT_EQ_INT(expect_type,ie_get_type(&v));\
	}while (0)

static void test_parse_null() {
	/* Deliberately start with another type: ie_parse must overwrite it. */
	TEST_ERROR(IE_PARSE_OK, IE_NULL, "null");
}

static void test_parse_true() {
	TEST_ERROR(IE_PARSE_OK, IE_TRUE, "true");
}

static void test_parse_false() {
	TEST_ERROR(IE_PARSE_OK, IE_FALSE, "false");
}

static void test_parse_invalid_value() {
	/* Only lowercase literals are valid: uppercase and mixed case are rejected. */
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "TRUE");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "True");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "truE");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "FALSE");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "False");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "falsE");
	/* Incomplete or unrelated literals. */
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "nul");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "?");

	/* ... */
	/* invalid number */
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "+0");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "+1");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, ".123"); /* at least one digit before '.' */
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "1.");   /* at least one digit after '.' */
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "INF");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "inf");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "NAN");
	TEST_ERROR(IE_PARSE_INVALID_VALUE, IE_NULL, "nan");
}

#define TEST_NUMBER(expect, json)\
    do {\
        ie_value value;\
        EXPECT_EQ_INT(IE_PARSE_OK, ie_parse(&value, json));\
        EXPECT_EQ_INT(IE_NUMBER, ie_get_type(&value));\
        EXPECT_EQ_DOUBLE(expect, ie_get_number(&value));\
    } while(0)

static void test_parse_number() {
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}
static void test_parse_root_not_singular() {
	TEST_ERROR(IE_PARSE_ROOT_NOT_SINGULAR, IE_NULL, "null x");
	/* invalid number */
	TEST_ERROR(IE_PARSE_ROOT_NOT_SINGULAR, IE_NULL, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
	TEST_ERROR(IE_PARSE_ROOT_NOT_SINGULAR, IE_NULL, "0x0");
	TEST_ERROR(IE_PARSE_ROOT_NOT_SINGULAR, IE_NULL, "0x123");
}
static void test_parse_number_too_big() {
	TEST_ERROR(IE_PARSE_NUMBER_TOO_BIG, IE_NULL, "1e309");
	TEST_ERROR(IE_PARSE_NUMBER_TOO_BIG, IE_NULL, "-1e309");
}

static void test_parse() {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_invalid_value();
	test_parse_number();
	test_parse_root_not_singular();
	test_parse_number_too_big();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}