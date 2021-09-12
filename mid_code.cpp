#include "compiler.h"

string temp = "$t";
string label = "$label";
string info = "$info";
string mid_RET = "RET";
string mid_ret = "ret";
string larray = "[]=";
string rarray = "=[]";
string mid_call = "call";
string mid_goto = "goto";
string mid_init = "init";
string mid_neg = "-";
string mid_assign = "=";
string mid_div = "/";
string mid_mul = "*";
string mid_add = "+";
string mid_sub = "-";
string mid_eql = "==";
string mid_neq = "!=";
string mid_lss = "<";
string mid_leq = "<=";
string mid_gre = ">";
string mid_geq = ">=";
string mid_scanf = "scanf";
string mid_printf = "printf";
string mid_push = "push";
string mid_exit = "exit";
string mid_const = "const";
string mid_int = "int";
string mid_char = "char";
string mid_void = "void";
string mid_var = "var";
string mid_para = "para";


void compiler::push_midcode(mid_operation operaton, string name1, string name2, string result_name) {
	mid_code_struct* mid = new mid_code_struct();
	mid->operation = operaton;
	mid->name1 = name1;
	mid->name2 = name2;
	mid->result_name = result_name;
	mid->lie = 0;
	mid_codes[mid_code_num] = mid;
	mid_code_num = mid_code_num + 1;
}

void compiler::push_midcode_special(mid_operation operaton, string name1, string name2, string name3, string result_name, int lie) {
	mid_code_struct* mid = new mid_code_struct();
	mid->operation = operaton;
	mid->name1 = name1;
	mid->name2 = name2;
	mid->array_2 = name3;
	mid->result_name = result_name;
	mid->lie = lie;
	mid_codes[mid_code_num] = mid;
	mid_code_num = mid_code_num + 1;
}

bool compiler::whether_op_is_num(string operation) {
	return ((operation.size() > 0) && ((operation[0] >= '0' && operation[0] <= '9') || operation[0] == '+' || operation[0] == '-'));
}

bool compiler::whether_op_is_char(string operation) {
	return ((operation.size() > 0) && (operation[0] == '_' || (operation[0] >= 'A' && operation[0] <= 'Z')
		|| (operation[0] >= 'a' && operation[0] <= 'z')));
}

bool compiler::whether_op_is_ret(string operation) {
	return ((operation.size() > 0) && (operation[0] == '#'));
}

bool compiler::whether_op_is_temp(string operation) {
	return ((operation.size() > 0) && (operation[0] == '$'));
}

void compiler::Deal_with_label(std::string* point) {
	std::stringstream label_string;
	label_string << label;
	label_string << cur_label++;
	*point = label_string.str();
}

void compiler::Deal_with_temp(std::string* point) {
	std::stringstream temp_string;
	temp_string << temp;
	temp_string << cur_temp++;
	*point = temp_string.str();
	push_sym_list(*point, VAR, INT, 0, line, -1, true);
}

int compiler::push_string(string s) {
	for (int i = 0; i < string_num; i++) {
		if (s == string_table[i]) {
			return i;
		}
	}
	string_table[string_num++] = s;
	return string_num - 1;
}

void compiler::int_to_string(string* s, int value) {
	std::stringstream temp_string;
	temp_string << value;
	*s = temp_string.str();
}

void compiler::printmidcode1() {
	for (int i = 0; i < mid_code_num; i++) {
		mid_code_struct* code = mid_codes[i];
		mid_operation tmpop = code->operation;
		string rst = code->result_name;
		string op1n = code->name1;
		string op2n = code->name2;
		string op3n = code->array_2;
		if (tmpop == REAL_PARA_OP) {
			mid_code_stream1 << mid_push << " " << rst << "\n";
		}
		else if (tmpop == RIGHT_ARRAY_OP) {
			mid_code_stream1 << rst << " = " << op1n << "[" << op2n << "]" << "\n";
		}
		else if (tmpop == LEFT_ARRAY_OP) {
			mid_code_stream1 << rst << "[" << op2n << "] " << "[" << op3n << "] " << mid_assign << " " << op1n << "\n";
		}
		else if (tmpop == RIGHT_DOUBLE_ARRAY_OP) {
			mid_code_stream1 << rst << " = " << op1n << "[" << op2n << "]" << "[" << op3n << "] " << "\n";
		}
		else if (tmpop == LEFT_DOUBLE_ARRAY_OP) {
			mid_code_stream1 << rst << "[" << op2n << "] " << mid_assign << " " << op1n << "\n";
		}
		else if (tmpop == USE_FUNC_OP) {
			mid_code_stream1 << mid_call << " " << rst << "\n";
		}
		else if (tmpop == GOTO_OP) {
			mid_code_stream1 << mid_goto << " " << rst << "\n";
		}
		else if (tmpop == FUNC_INIT_OP) {
			mid_code_stream1 << mid_init << " " << rst << "\n";
		}
		else if (tmpop == FUNC_RETURN_OP) {
			mid_code_stream1 << mid_ret << " " << rst << "\n";
		}
		else if (tmpop == MUL_OP) {
			mid_code_stream1 << rst << " " << mid_assign << " " << op1n << " " << mid_mul << " " << op2n << "\n";
		}
		else if (tmpop == DIV_OP) {
			mid_code_stream1 << rst << " " << mid_assign << " " << op1n << " " << mid_div << " " << op2n << "\n";
		}
		else if (tmpop == ADD_OP) {
			mid_code_stream1 << rst << " " << mid_assign << " " << op1n << " " << mid_add << " " << op2n << "\n";
		}
		else if (tmpop == SUB_OP) {
			mid_code_stream1 << rst << " " << mid_assign << " " << op1n << " " << mid_sub << " " << op2n << "\n";
		}
		else if (tmpop == EQL_OP) {
			mid_code_stream1 << "if " << op1n << " " << mid_eql << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == NEQ_OP) {
			mid_code_stream1 << "if " << op1n << " " << mid_neq << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == LSS_OP) {
			mid_code_stream1 << "if " << op1n << " " << mid_lss << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == LEQ_OP) {
			mid_code_stream1 << "if " << op1n << " " << mid_leq << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == GRE_OP) {
			mid_code_stream1 << "if " << op1n << " " << mid_gre << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == GEQ_OP) {
			mid_code_stream1 << "if " << op1n << " " << mid_geq << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == SCANF_OP) {
			mid_code_stream1 << mid_scanf << " " << op2n << " " << rst << "\n";
		}
		else if (tmpop == PRINTF_OP) {
			mid_code_stream1 << mid_printf << " " << op1n << " " << op2n << " " << rst << "\n";
		}
		else if (tmpop == EXIT_OP) {
			mid_code_stream1 << mid_exit << " " << "\n";
		}
		else if (tmpop == LAB_OP) {
			mid_code_stream1 << rst << ":" << "\n";
		}
	}
}

void compiler::printmidcode2() {
	for (int i = 0; i < mid_code_num; i++) {
		mid_code_struct* code = mid_codes[i];
		mid_operation tmpop = code->operation;
		string rst = code->result_name;
		string op1n = code->name1;
		string op2n = code->name2;
		string op3n = code->array_2;
		if (tmpop == REAL_PARA_OP) {
			mid_code_stream2 << mid_push << " " << rst << "\n";
		}
		else if (tmpop == RIGHT_ARRAY_OP) {
			mid_code_stream2 << rst << " = " << op1n << "[" << op2n << "]" << "\n";
		}
		else if (tmpop == LEFT_ARRAY_OP) {
			mid_code_stream2 << rst << "[" << op2n << "] " << "[" << op3n << "] " << mid_assign << " " << op1n << "\n";
		}
		else if (tmpop == RIGHT_DOUBLE_ARRAY_OP) {
			mid_code_stream2 << rst << " = " << op1n << "[" << op2n << "]" << "[" << op3n << "] " << "\n";
		}
		else if (tmpop == LEFT_DOUBLE_ARRAY_OP) {
			mid_code_stream2 << rst << "[" << op2n << "] " << mid_assign << " " << op1n << "\n";
		}
		else if (tmpop == USE_FUNC_OP) {
			mid_code_stream2 << mid_call << " " << rst << "\n";
		}
		else if (tmpop == GOTO_OP) {
			mid_code_stream2 << mid_goto << " " << rst << "\n";
		}
		else if (tmpop == FUNC_INIT_OP) {
			mid_code_stream2 << mid_init << " " << rst << "\n";
		}
		else if (tmpop == FUNC_RETURN_OP) {
			mid_code_stream2 << mid_ret << " " << rst << "\n";
		}
		else if (tmpop == MUL_OP) {
			mid_code_stream2 << rst << " " << mid_assign << " " << op1n << " " << mid_mul << " " << op2n << "\n";
		}
		else if (tmpop == DIV_OP) {
			mid_code_stream2 << rst << " " << mid_assign << " " << op1n << " " << mid_div << " " << op2n << "\n";
		}
		else if (tmpop == ADD_OP) {
			mid_code_stream2 << rst << " " << mid_assign << " " << op1n << " " << mid_add << " " << op2n << "\n";
		}
		else if (tmpop == SUB_OP) {
			mid_code_stream2 << rst << " " << mid_assign << " " << op1n << " " << mid_sub << " " << op2n << "\n";
		}
		else if (tmpop == EQL_OP) {
			mid_code_stream2 << "if " << op1n << " " << mid_eql << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == NEQ_OP) {
			mid_code_stream2 << "if " << op1n << " " << mid_neq << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == LSS_OP) {
			mid_code_stream2 << "if " << op1n << " " << mid_lss << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == LEQ_OP) {
			mid_code_stream2 << "if " << op1n << " " << mid_leq << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == GRE_OP) {
			mid_code_stream2 << "if " << op1n << " " << mid_gre << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == GEQ_OP) {
			mid_code_stream2 << "if " << op1n << " " << mid_geq << " " << op2n << " then goto " << rst << "\n";
		}
		else if (tmpop == SCANF_OP) {
			mid_code_stream2 << mid_scanf << " " << op2n << " " << rst << "\n";
		}
		else if (tmpop == PRINTF_OP) {
			mid_code_stream2 << mid_printf << " " << op1n << " " << op2n << " " << rst << "\n";
		}
		else if (tmpop == EXIT_OP) {
			mid_code_stream2 << mid_exit << " " << "\n";
		}
		else if (tmpop == LAB_OP) {
			mid_code_stream2 << rst << ":" << "\n";
		}
	}
}