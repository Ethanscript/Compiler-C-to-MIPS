#include "compiler.h"

void compiler::error_collect(error_type error_type, int error_line) {
	error new_error;
	new_error.error_line = error_line;
	new_error.error_type = error_type;
	switch (error_type)
	{
	case ILLGEAL_CHAR_ERROR: {
		new_error.error_output_type = "a";
		break;
	}
	case REDEFINE_NAME_ERROR: {
		new_error.error_output_type = "b";
		break;
	}
	case UNDEFIEN_NAME_ERROR: {
		new_error.error_output_type = "c";
		break;
	}
	case FUNC_PARA_NUM_ERROR: {
		new_error.error_output_type = "d";
		break;
	}
	case FUNC_PARA_TYPE_ERROR: {
		new_error.error_output_type = "e";
		break;
	}
	case CONDITION_ERROR: {
		new_error.error_output_type = "f";
		break;
	}
	case NON_RETURN_FUNC_ERROR: {
		new_error.error_output_type = "g";
		break;
	}
	case HAVE_RETURN_FUNC_ERROR: {
		new_error.error_output_type = "h";
		break;
	}
	case ARRAY_INDEX_INTEGER_ERROR: {
		new_error.error_output_type = "i";
		break;
	}
	case CONST_VALUE_CHANGE_ERROR: {
		new_error.error_output_type = "j";
		break;
	}
	case SEMICN_ERROR: {
		new_error.error_output_type = "k";
		break;
	}
	case RPARENT_ERROR: {
		new_error.error_output_type = "l";
		break;
	}
	case RBRACK_ERROR: {
		new_error.error_output_type = "m";
		break;
	}
	case ARRAY_INIT_NUM_ERROR: {
		new_error.error_output_type = "n";
		break;
	}
	case CONST_VALUE_TYPE_ERROR: {
		new_error.error_output_type = "o";
		break;
	}
	case DEFAULT_DISMISS_ERROR: {
		new_error.error_output_type = "p";
		break;
	}
	default:
		break;
	}
	error_record[error_num] = new_error;
	error_num = error_num + 1;
}

void compiler::error_output() {
	for (int i = 0; i < error_num; i++) {
		out << error_record[i].error_line << " " << error_record[i].error_output_type << "\n";
	}
}

void compiler::jump_to_right_sym() {
	while (sym != "COMMA" && sym != "SEMICN") {
		getsym();
	}
}