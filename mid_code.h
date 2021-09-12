#pragma once
#include "compiler.h"

enum mid_operation
{
	ADD_OP, SUB_OP, MUL_OP, DIV_OP, 
	SCANF_OP, PRINTF_OP, FUNC_INIT_OP,
	LEQ_OP, LSS_OP, GEQ_OP, GRE_OP,
	NEQ_OP, EQL_OP, LAB_OP, GOTO_OP,
	FUNC_RETURN_OP, REAL_PARA_OP,
	USE_FUNC_OP, EXIT_OP, LEFT_ARRAY_OP,
	RIGHT_ARRAY_OP, LEFT_DOUBLE_ARRAY_OP,
	RIGHT_DOUBLE_ARRAY_OP

};

typedef struct {
	mid_operation operation;
	std::string name1;
	std::string name2;
	std::string result_name;

	std::string array_2;
	int lie;
}mid_code_struct;