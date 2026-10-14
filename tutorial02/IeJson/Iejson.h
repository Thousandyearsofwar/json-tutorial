#pragma once
typedef enum {
	IE_NULL,
	IE_FALSE,
	IE_TRUE,
	IE_NUMBER,
	IE_STRING,
	IE_ARRAY,
	IE_OBJECT
} ie_type;

typedef struct
{
	double numberValue;
	ie_type type;
} ie_value;

enum
{
	IE_PARSE_OK = 0,
	IE_PARSE_EXPECT_VALUE,
	IE_PARSE_INVALID_VALUE,
	IE_PARSE_ROOT_NOT_SINGULAR,
	IE_PARSE_NUMBER_TOO_BIG
};




int ie_parse(ie_value* value, const char* jsonRawData);

ie_type ie_get_type(const ie_value* value);

double ie_get_number(const ie_value* value);