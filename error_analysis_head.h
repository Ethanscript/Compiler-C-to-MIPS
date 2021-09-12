#pragma once
#include "compiler.h"

enum error_type {
	ILLGEAL_CHAR_ERROR,
	REDEFINE_NAME_ERROR,
	UNDEFIEN_NAME_ERROR,
	FUNC_PARA_NUM_ERROR,
	FUNC_PARA_TYPE_ERROR,
	CONDITION_ERROR,
	NON_RETURN_FUNC_ERROR,
	HAVE_RETURN_FUNC_ERROR,
	ARRAY_INDEX_INTEGER_ERROR,
	CONST_VALUE_CHANGE_ERROR,
	SEMICN_ERROR,
	RPARENT_ERROR,
	RBRACK_ERROR,
	ARRAY_INIT_NUM_ERROR,
	CONST_VALUE_TYPE_ERROR,	//switch 变量定义初始化
	DEFAULT_DISMISS_ERROR
};

typedef struct {
	int error_line;
	error_type error_type;
	std::string error_output_type;
}error;
