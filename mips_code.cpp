#include "compiler.h"
#include "mips_code.h"

std::string T7 = "$t7";
std::string T8 = "$t8";
std::string T9 = "$t9";
std::string SP = "$sp";
std::string FP = "$fp";
std::string GP = "$gp";
std::string RA = "$ra";
std::string V0 = "$v0";
std::string A0 = "$a0";
std::string R0 = "$zero";

void compiler::push_mipscode(mips_code_struct* mips_code) {
	mips_codes[mips_code_num++] = mips_code;
}

void compiler::initial_regs() {
	for (int i = 0; i < 14; i++) {		//$t0 -> $t9, $s0 -> $s7
		regs[i] = "";
	}
}

void compiler::initial_ascii() {
	for (int i = 0; i < string_num; i++) {
		std::stringstream temp_string;
		temp_string << "$STRING" << i << ":.asciiz \"";
		for (int j = 0; j < string_table[i].size(); j++) {
			if (string_table[i][j] == '\\') {
				temp_string << "\\";
			}
			temp_string << string_table[i][j];
		}
		temp_string << "\"";
		mips_code_struct* mips_code = new mips_code_struct();
		mips_code->operation = MIPS_STRING;
		mips_code->string_buff = temp_string.str();
		push_mipscode(mips_code);
	}
	std::stringstream temp_string;
	temp_string << "$STRING" << string_num << ":.asciiz \"\\n\"";
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_STRING;
	mips_code->string_buff = temp_string.str();
	push_mipscode(mips_code);

	string_num++;
}

void compiler::Deal_with_mips() {
	initial_regs();

	std::stringstream temp_string;
	temp_string << ".data";
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_STRING;
	mips_code->string_buff = temp_string.str();
	push_mipscode(mips_code);

	initial_ascii();
	
	std::stringstream temp_string1;
	temp_string1 << ".text";
	mips_code_struct* mips_code1 = new mips_code_struct();
	mips_code1->operation = MIPS_STRING;
	mips_code1->string_buff = temp_string1.str();
	push_mipscode(mips_code1);

	for (int i = 0; i < mid_code_num; i++) {
		mid_code_struct* mid_code = mid_codes[i];
		switch (mid_code->operation)
		{
		case (FUNC_INIT_OP):
			Deal_with_func(mid_code);
			break;
		case (ADD_OP):
			Deal_with_add_sub(mid_code);
			break;
		case (SUB_OP):
			Deal_with_add_sub(mid_code);
			break;
		case (MUL_OP):
			Deal_with_mul_div(mid_code);
			break;
		case (DIV_OP):
			Deal_with_mul_div(mid_code);
			break;
		case (PRINTF_OP):
			Deal_with_printf(mid_code);
			break;
		case (SCANF_OP):
			Deal_with_scanf(mid_code);
			break;
		case (LEQ_OP):
		case (LSS_OP):
		case (GEQ_OP):
		case (GRE_OP):
		case (NEQ_OP):
		case (EQL_OP):
			Deal_with_compare(mid_code);
			break;
		case (LAB_OP):
			Deal_with_label(mid_code);
			break;
		case (GOTO_OP):
			Deal_with_goto(mid_code);
			break;
		case (FUNC_RETURN_OP):
			Deal_with_func_return(mid_code);
			break;
		case (REAL_PARA_OP):
			Deal_with_real_para(mid_code);
			break;
		case (USE_FUNC_OP):
			Deal_with_use_func(mid_code);
			break;
		case (EXIT_OP):
			Deal_with_exit(mid_code);
			break;
		case (LEFT_ARRAY_OP):
			Deal_with_left_array(mid_code);
			break;
		case (RIGHT_ARRAY_OP):
			Deal_with_right_array(mid_code);
			break;
		case (LEFT_DOUBLE_ARRAY_OP):
			Deal_with_left_double_array(mid_code);
			break;
		case (RIGHT_DOUBLE_ARRAY_OP):
			Deal_with_right_double_array(mid_code);
			break;
		default:
			break;
		}
	}
}

void compiler::def_global_value(string name, symbol** sym, bool* global) {
	if (cur_func != -1) {
		int max_item = func_item_num[cur_func];
		for (int i = 0; i < max_item; i++) {	//如果在
			symbol* tmp_sym = func_table[cur_func][i];
			if (tmp_sym->name == name) {
				*global = false;
				*sym = tmp_sym;
				return;
			}
		}	
		for (int i = 0; i < symbol_num - 1; i++) {
			symbol* tmp_sym = symbol_list[i];
		
			if (tmp_sym->name == name) {		//???
				*global = true;
				*sym = tmp_sym;
				return;
			}
		}
	}
	else {
		for (int i = 0; i < symbol_num - 1; i++) {
			symbol* tmp_sym = symbol_list[i];
			if (tmp_sym->name == name) {		
				*global = true;
				*sym = tmp_sym;
				return;
			}
		}
	}
}

void compiler::choose_name1_name2_reg(string* rs, string* reg) {
	if (whether_op_is_ret(*rs)) {
		*reg = V0;
	}
	else if (whether_op_is_num(*rs)) {
		if ((*rs)[0] == '0') {
			*reg = R0;
		}
		else {
			LI_STR (*reg, *rs);
		}
	}
	else {
		bool global = false;
		symbol* sym = 0;
		def_global_value(*rs, &sym, &global);
		if (sym->reg == -1) {
			int offset = sym->address;
			if (global == true) {
				offset = 4 * offset;
				LW (*reg, offset, GP);
			}
			else {
				offset = (-4) * offset;
				LW (*reg, offset, FP);
			}
		}
	}
}

void compiler::choose_result_name_reg(string* rd, string* reg) {
	if (whether_op_is_ret(*rd)) {
		*reg = V0;
		return;
	}
	else {
		bool global = false;
		symbol* sym = 0;
		def_global_value(*rd, &sym, &global);
	}
}

void compiler::store_word(string rd, string reg) {
	if (!whether_op_is_ret(rd)) {
		symbol* sym = 0;
		bool global = false;
		def_global_value(rd, &sym, &global);
		if (sym && sym->reg == -1) {
			int offset = 0;
			if (global == true) {
				offset = 4 * sym->address;
				SW (reg, offset, GP);
			}
			else {
				offset = (-4) * sym->address;
				SW (reg, offset, FP);
			}
		}
	}
}

void compiler::Deal_with_mul_div(mid_code_struct* code) {
	string name1 = code->name1;
	string name2 = code->name2;
	string result = code->result_name;
	string reg1 = T9;
	string reg2 = T8;
	string result_reg = T7;
	if (code->operation == MUL_OP) {
		choose_name1_name2_reg(&name1, &reg1);
		choose_name1_name2_reg(&name2, &reg2);
		choose_result_name_reg(&result, &result_reg);
		MUL (result_reg, reg1, reg2);
		store_word(result, result_reg);
	}
	else {
		if (!whether_op_is_num(name2) || (whether_op_is_num(name2) && !whether_is_pow2(abs(std::stoi(name2))))) {
			choose_name1_name2_reg(&name1, &reg1);
			choose_name1_name2_reg(&name2, &reg2);
			choose_result_name_reg(&result, &result_reg);
			DIV(reg1, reg2);
			MFLO(result_reg);
			store_word(result, result_reg);
		}
		else {
			int num = std::stoi(name2);
			if (num >= 0) {
				choose_name1_name2_reg(&name1, &reg1);
				string label1 = "";
				Deal_with_label(&label1);
				string label2 = "";
				Deal_with_label(&label2);

				BLTZ(reg1, label1);
				SRL(reg1, reg1, get_log2_index(num));
				store_word(result, reg1);
				J(label2);

				LABEL(label1);
				SUBU_REG(reg1, R0, reg1);
				SRL(reg1, reg1, get_log2_index(num));
				SUBU_REG(reg1, R0, reg1);
				store_word(result, reg1);
				LABEL(label2);

			}
			else {
				int org_num = num;
				choose_name1_name2_reg(&name1, &reg1);
				string label1 = "";
				Deal_with_label(&label1);
				string label2 = "";
				Deal_with_label(&label2);

				BLTZ(reg1, label1);
				num = -num;
				SRL(reg1, reg1, get_log2_index(num));
				SUBU_REG(reg1, R0, reg1);
				store_word(result, reg1);
				J(label2);

				LABEL(label1);
				SUBU_REG(reg1, R0, reg1);
				num = -org_num;
				SRL(reg1, reg1, get_log2_index(num));
				store_word(result, reg1);
				LABEL(label2);
			}
		}
	}
}

void compiler::Deal_with_add_sub(mid_code_struct* code) {
	string name1 = code->name1;
	string name2 = code->name2;
	string result = code->result_name;
	string reg1 = T9;
	string reg2 = T8;
	string result_reg = T7;
	if (whether_op_is_num(name1)) {
		if (whether_op_is_num(name2)) {
			int value1 = stoi(name1);
			int value2 = stoi(name2);
			int value = 0;
			if (code->operation == ADD_OP) {
				value = value1 + value2;
			}
			else {
				value = value1 - value2;
			}
			choose_result_name_reg(&result, &result_reg);
			LI_IMM (result_reg, value);
			store_word(result, result_reg);
		}
		else if (code->operation == ADD_OP) {
			choose_name1_name2_reg(&name2, &reg2);
			choose_result_name_reg(&result, &result_reg);
			ADDU_REG(result_reg, reg2, name1);
			store_word(result, result_reg);
		}
		else {
			choose_name1_name2_reg(&name1, &reg1);
			choose_name1_name2_reg(&name2, &reg2);
			choose_result_name_reg(&result, &result_reg);
			SUBU_REG (result_reg, reg1, reg2);
			store_word(result, result_reg);
		}
	}
	else {
		choose_name1_name2_reg(&name1, &reg1);
		if (whether_op_is_num(name2)) {
			choose_result_name_reg(&result, &result_reg);
			if (code->operation == ADD_OP) {
				ADDU_REG (result_reg, reg1, name2);
				store_word(result, result_reg);
			}
			else if (code->operation == SUB_OP) {
				SUBU_REG (result_reg, reg1, name2);
				store_word(result, result_reg);
			}
		}
		else {
			choose_name1_name2_reg(&name2, &reg2);
			choose_result_name_reg(&result, &result_reg);
			if (code->operation == ADD_OP) {
				ADDU_REG (result_reg, reg1, reg2);
				store_word(result, result_reg);
			}
			else if (code->operation == SUB_OP) {
				SUBU_REG (result_reg, reg1, reg2);
				store_word(result, result_reg);
			}
		}
	}
}

void compiler::Deal_with_scanf(mid_code_struct* code) {
	if (code->name2 == "int") {
		LI_IMM (V0, 5);
		SYSCALL();
		string result_reg = T7;
		choose_result_name_reg(&(code->result_name), &result_reg);
		ADDU_REG (result_reg, V0, R0);
		store_word(code->result_name, result_reg);
	}
	else if (code->name2 == "char") {
		LI_IMM (V0, 12);
		SYSCALL();
		string result_reg = T7;
		choose_result_name_reg(&(code->result_name), &result_reg);
		ADDU_REG (result_reg, V0, R0);
		store_word(code->result_name, result_reg);
	}
}

void compiler::Deal_with_printf(mid_code_struct* code) {
	if (code->name2 == "int") {
		LI_IMM (V0, 1);
		string result_reg = T7;
		choose_name1_name2_reg(&(code->result_name), &result_reg);
		ADDU_REG (A0, result_reg, R0);
		SYSCALL();
	}
	else if (code->name2 == "char") {
		LI_IMM (V0, 11);
		string result_reg = T7;
		choose_name1_name2_reg(&(code->result_name), &result_reg);
		ADDU_REG (A0, result_reg, R0);
		SYSCALL();
	}
	else if (code->name2 == "0") {
		LI_IMM (V0, 4);
		std::stringstream temp_string;
		temp_string << "$STRING" << code->result_name;
		LA (A0, temp_string.str());
		SYSCALL();
	}
	if (code->name1 == "-1") {
		LI_IMM (V0, 4);
		std::stringstream temp_string;
		temp_string << "$STRING" << string_num - 1;
		LA (A0, temp_string.str());
		SYSCALL();
	}
}

symbol* compiler::func_find(string name) {
	symbol* tmp_sym = 0;
	for (int i = 0; i < symbol_num; i++) {
		if (symbol_list[i]->item_class_type == FUNC) {
			if (symbol_list[i]->name == name) {
				tmp_sym = symbol_list[i];
				return tmp_sym;
			}
		}
	}
	return tmp_sym;
}

void compiler::Deal_with_func(mid_code_struct* code) {
	if (enter_first_func == false) {
		J ("main");
		enter_first_func = true;
	}
	
	cur_func++;
	symbol* sym = func_find(code->result_name);
	Deal_with_label(code);
	int cur_address = func_address[sym->position];
	int para_num = sym->para_num;	//??????????????????????????????????????????????
	if (para_num != 0) {
		ADDU_IMM (SP, SP, para_num * 4);
	}



	for (int i = 0; i < 14; i++) {
		std::stringstream ss;
		int index = i % (14 / 2);
		string temp;
		if (i >= 7) {
			temp = "$s";
		}
		else {
			temp = "$t";
		}
		ss << temp << index;
		//SW (ss.str(), -cur_address * 4, SP);
		cur_address++;
	}

	SW (FP, -cur_address * 4, SP);
	cur_address++;
	SW (RA, -cur_address * 4, SP);
	cur_address++;
	ADDU_REG (FP, SP, R0);
	ADDU_IMM (SP, SP, -cur_address * 4);
}

void compiler::Deal_with_compare(mid_code_struct* code) {
	string exp_value_1 = code->name1;
	string exp_value_2 = code->name2;
	string result = code->result_name;
	string reg_1 = T9;
	string reg_2 = T8;
	string result_reg = T7;
	if (code->operation == EQL_OP || code->operation == NEQ_OP) {
		if (whether_op_is_num(exp_value_1)) {
			if (whether_op_is_num(exp_value_2)) {
				int value1 = stoi(exp_value_1);
				int value2 = stoi(exp_value_2);
				value1 = value1 - value2;
				string const_value = to_string(value1);
				choose_name1_name2_reg(&const_value, &reg_1);
				reg_2 = R0;
			}
			else {
				choose_name1_name2_reg(&exp_value_1, &reg_1);
				choose_name1_name2_reg(&exp_value_2, &reg_2);
			}
		}
		else {
			choose_name1_name2_reg(&exp_value_1, &reg_1);
			choose_name1_name2_reg(&exp_value_2, &reg_2);
		}

		if (code->operation == EQL_OP) {
			BEQ (reg_1, reg_2, code->result_name);
		}
		else {
			BNE (reg_1, reg_2, code->result_name);
		}
	}
	else {
		if (whether_op_is_num(exp_value_1)) {
			if (whether_op_is_num(exp_value_2)) {
				int value1 = stoi(exp_value_1);
				int value2 = stoi(exp_value_2);
				value1 = value1 - value2;
				string const_value = to_string(value1);
				choose_name1_name2_reg(&const_value, &result_reg);
			}
			else {
				choose_name1_name2_reg(&exp_value_1, &reg_1);
				choose_name1_name2_reg(&exp_value_2, &reg_2);
				SUBU_REG (result_reg, reg_1, reg_2);
			}
		}
		else {
			choose_name1_name2_reg(&exp_value_1, &reg_1);
			if (whether_op_is_num(exp_value_2)) {
				SUBU_REG (result_reg, reg_1, exp_value_2);
			}
			else {
				choose_name1_name2_reg(&exp_value_2, &reg_2);
				SUBU_REG (result_reg, reg_1, reg_2);
			}
		}
		if (code->operation == GRE_OP) {
			BGTZ (result_reg, result);
		}
		else if (code->operation == GEQ_OP) {
			BGEZ (result_reg, result);
		}
		else if (code->operation == LSS_OP) {
			BLTZ (result_reg, result);
		}
		else if (code->operation == LEQ_OP) {
			BLEZ (result_reg, result);
		}
	}
}

void compiler::Deal_with_label(mid_code_struct* code) {
	LABEL(code->result_name);
}

void compiler::Deal_with_goto(mid_code_struct* code) {
	J (code->result_name);
}

void compiler::Deal_with_func_return(mid_code_struct* code) {
	symbol* sym = func_find(code->result_name);
	int cur_address = 1;
	LW (RA, cur_address * 4, SP);
	cur_address++;
	LW (FP, cur_address * 4, SP);
	cur_address++;
	initial_regs();
	
	for (int i = 14 - 1; i >= 0; i--) {
		std::stringstream ss;
		int index = i % (14 / 2);
		string temp;
		if (i >= 7) {
			temp = "$s";
		}
		else {
			temp = "$t";
		}
		ss << temp << index;
		//LW (ss.str(), cur_address * 4, SP);
		cur_address++;
	}
	cur_address += (func_address[sym->position] - 1);
	ADDU_IMM (SP, SP, cur_address * 4);
	JR (RA);
}

void compiler::Deal_with_real_para(mid_code_struct* code) {
	string result_reg = T7;
	choose_name1_name2_reg(&(code->result_name), &(result_reg));
	SW (result_reg, 0, SP);
	ADDU_IMM (SP, SP, -4);
}

void compiler::Deal_with_use_func(mid_code_struct* code) {
	JAL (code->result_name);
}

void compiler::Deal_with_exit(mid_code_struct* code) {
	LI_IMM (V0, 10);
	SYSCALL();
}

void compiler::Deal_with_left_array(mid_code_struct* code) {
	string value = code->name1;
	string index_1 = code->name2;
	string array_name = code->result_name;
	string reg_1 = T9;
	string reg_2 = T8;
	symbol* sym = 0;
	bool global = false;
	def_global_value(array_name, &sym, &global);
	choose_name1_name2_reg(&index_1, &reg_2);
	ADDU_IMM (T7, reg_2, sym->address);
	SLL (T7, T7, 2);
	choose_name1_name2_reg(&value, &reg_1);
	if (global == true) {
		ADDU_REG (T7, T7, GP);
		SW (reg_1, 0, T7);
	}
	else {
		SUBU_REG (T7, FP, T7);
		SW (reg_1, 0, T7);
	}
}

void compiler::Deal_with_right_array(mid_code_struct* code) {
	string array_name = code->name1;
	string index_1 = code->name2;
	string value = code->result_name;
	string reg_2 = T8;
	string result_reg = T7;
	symbol* sym = 0;
	bool global = false;
	def_global_value(array_name, &sym, &global);
	choose_name1_name2_reg(&index_1, &reg_2);
	ADDU_IMM (T9, reg_2, sym->address);
	SLL (T9, T9, 2);
	choose_result_name_reg(&value, &result_reg);
	if (global == true) {
		ADDU_REG (T9, T9, GP);
		LW (result_reg, 0, T9);
	}
	else {
		SUBU_REG (T9, FP, T9);
		LW (result_reg, 0, T9);
	}
	store_word(value, result_reg);
}

void compiler::Deal_with_left_double_array(mid_code_struct* code) {
	string value = code->name1;	//值
	string index_1 = code->name2;	//x
	string index_2 = code->array_2;	//y
	string array_name = code->result_name;	//a
	int lie_expected = code->lie;
	string reg_1 = T9;
	string reg_2 = T8;
	symbol* sym = 0;
	bool global = false;

	def_global_value(array_name, &sym, &global);
	LI_STR (T8, to_string(lie_expected));
	choose_name1_name2_reg(&index_1, &reg_1);
	MUL (T7, reg_1, T8);

	reg_1 = T9;	//重新初始化
	choose_name1_name2_reg(&index_2, &reg_1);
	ADDU_REG (T8, T7, reg_1);
	ADDU_IMM (T7, T8, sym->address);
	SLL (T7, T7, 2);

	reg_1 = T9;	//重新初始化
	choose_name1_name2_reg(&value, &reg_1);
	if (global == true) {
		ADDU_REG (T7, T7, GP);
		SW (reg_1, 0, T7);
	}
	else {
		SUBU_REG (T7, FP, T7);
		SW (reg_1, 0, T7);
	}
}

void compiler::Deal_with_right_double_array(mid_code_struct* code) {
	string array_name = code->name1;	//a
	string index_1 = code->name2;	//x
	string index_2 = code->array_2;	//y
	string value = code->result_name;	//值

	int lie_expected = code->lie;
	string reg_2 = T8;
	string reg_1 = T9;
	string result_reg = T7;
	symbol* sym = 0;
	bool global = false;

	def_global_value(array_name, &sym, &global);
	LI_STR (T8, to_string(lie_expected));
	choose_name1_name2_reg(&index_1, &reg_1);
	MUL (T7, reg_1, T8);

	reg_1 = T9;	//重新初始化
	choose_name1_name2_reg(&index_2, &reg_1);
	ADDU_REG (T8, T7, reg_1);
	ADDU_IMM (T7, T8, sym->address);
	SLL (T7, T7, 2);

	choose_result_name_reg(&(code->result_name), &result_reg);
	if (global == true) {
		ADDU_REG (T7, T7, GP);
		LW (result_reg, 0, T7);
	}
	else {
		SUBU_REG (T7, FP, T7);
		LW (result_reg, 0, T7);
	}
	store_word(value, result_reg);
}

void compiler::LW (string rs, int offset, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_LW;
	mips_code->rs = rs;
	mips_code->offset = offset;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::SW (string rs, int offset, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_SW;
	mips_code->rs = rs;
	mips_code->offset = offset;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::ADDU_IMM (string rd, string rt, int imm_num) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_ADDU_IMM;
	mips_code->rd = rd;
	mips_code->rt = rt;
	mips_code->imm_num = imm_num;
	push_mipscode(mips_code);
}

void compiler::ADDU_REG(string rd, string rs, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_ADDU_REG;
	mips_code->rd = rd;
	mips_code->rs = rs;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::SUBU_REG(string rd, string rs, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_SUBU_REG;
	mips_code->rd = rd;
	mips_code->rs = rs;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::SLL (string rd, string rt, int imm_num) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_SLL;
	mips_code->rd = rd;
	mips_code->rt = rt;
	mips_code->imm_num = imm_num;
	push_mipscode(mips_code);
}

void compiler::SRL(string rd, string rt, int imm_num) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_SRL;
	mips_code->rd = rd;
	mips_code->rt = rt;
	mips_code->imm_num = imm_num;
	push_mipscode(mips_code);
}

void compiler::MUL (string rd, string rs, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_MUL;
	mips_code->rd = rd;
	mips_code->rs = rs;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::DIV (string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_DIV;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::MFLO (string target) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_MFLO;
	mips_code->target = target;
	push_mipscode(mips_code);
}

void compiler::BEQ (string rd, string rs, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_BEQ;
	mips_code->rd = rd;
	mips_code->rs = rs;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::BNE (string rd, string rs, string rt) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_BNE;
	mips_code->rd = rd;
	mips_code->rs = rs;
	mips_code->rt = rt;
	push_mipscode(mips_code);
}

void compiler::BGEZ (string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_BGEZ;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::BGTZ(string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_BGTZ;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::BLEZ(string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_BLEZ;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::BLTZ(string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_BLTZ;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::J (string target) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_J;
	mips_code->target = target;
	push_mipscode(mips_code);
}

void compiler::JAL (string target) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_JAL;
	mips_code->target = target;
	push_mipscode(mips_code);
}

void compiler::JR (string target) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_JR;
	mips_code->target = target;
	push_mipscode(mips_code);
}

void compiler::LI_IMM (string rs, int num) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_LI_IMM;
	mips_code->rs = rs;
	mips_code->num = num;
	push_mipscode(mips_code);
}

void compiler::LI_STR (string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_LI_STR;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::LA (string rs, string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_LA;
	mips_code->rs = rs;
	mips_code->label = label;
	push_mipscode(mips_code);
}

void compiler::SYSCALL() {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_SYSCALL;
	push_mipscode(mips_code);
}

void compiler::LABEL(string label) {
	mips_code_struct* mips_code = new mips_code_struct();
	mips_code->operation = MIPS_LABEL;
	mips_code->label = label;
	push_mipscode(mips_code);
}



void compiler::print_mipscode() {
	for (int i = 0; i < mips_code_num; i++) {
		switch (mips_codes[i]->operation)
		{
		case (MIPS_LW): {
			mips_code_stream << "lw" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->offset << "(" << mips_codes[i]->rt << ")" << "\n";
			break;
		}
		case (MIPS_SW): {
			mips_code_stream << "sw" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->offset << "(" << mips_codes[i]->rt << ")" << "\n";
			break;
		}
		case (MIPS_ADDU_IMM): {
			mips_code_stream << "addu" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rt << ", " << mips_codes[i]->imm_num << "\n";
			break;
		}
		case (MIPS_ADDU_REG): {
			mips_code_stream << "addu" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rs << ", " << mips_codes[i]->rt << "\n";
			break;
		}
		case (MIPS_SUBU_REG): {
			mips_code_stream << "subu" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rs << ", " << mips_codes[i]->rt << "\n";
			break;
		}
		case (MIPS_SLL): {
			mips_code_stream << "sll" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rt << ", " << mips_codes[i]->imm_num << "\n";
			break;
		}
		case (MIPS_SRL): {
			mips_code_stream << "srl" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rt << ", " << mips_codes[i]->imm_num << "\n";
			break;
		}
		case (MIPS_MUL): {
			mips_code_stream << "mul" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rs << ", " << mips_codes[i]->rt << "\n";
			break;
		}
		case (MIPS_DIV): {
			mips_code_stream << "div" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_MFLO): {
			mips_code_stream << "mflo" << " " << mips_codes[i]->target << "\n";
			break;
		}
		case (MIPS_BEQ): {
			mips_code_stream << "beq" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rs << ", " << mips_codes[i]->rt << "\n";
			break;
		}
		case (MIPS_BNE): {
			mips_code_stream << "bne" << " " << mips_codes[i]->rd << ", " << mips_codes[i]->rs << ", " << mips_codes[i]->rt << "\n";
			break;
		}
		case (MIPS_BGEZ): {
			mips_code_stream << "bgez" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_BGTZ): {
			mips_code_stream << "bgtz" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_BLEZ): {
			mips_code_stream << "blez" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_BLTZ): {
			mips_code_stream << "bltz" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_J): {
			mips_code_stream << "j" << " " << mips_codes[i]->target << "\n";
			break;
		}
		case (MIPS_JAL): {
			mips_code_stream << "jal" << " " << mips_codes[i]->target << "\n";
			break;
		}
		case (MIPS_JR): {
			mips_code_stream << "JR" << " " << mips_codes[i]->target << "\n";
			break;
		}
		case (MIPS_LI_IMM): {
			mips_code_stream << "li" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->num << "\n";
			break;
		}
		case (MIPS_LI_STR): {
			mips_code_stream << "li" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_LA): {
			mips_code_stream << "la" << " " << mips_codes[i]->rs << ", " << mips_codes[i]->label << "\n";
			break;
		}
		case (MIPS_SYSCALL): {
			mips_code_stream << "syscall" << "\n";
			break;
		}
		case (MIPS_LABEL): {
			mips_code_stream << mips_codes[i]->label << ":" << "\n";
			break;
		}
		case (MIPS_STRING): {
			mips_code_stream << mips_codes[i]->string_buff << "\n";
			break;
		}
		default:
			break;
		}
	}
}


/*
void compiler::print_mipscode() {
		mips_code_stream << ".text" << "\n";
		mips_code_stream << "li $t1, 1" << "\n";
}
*/
