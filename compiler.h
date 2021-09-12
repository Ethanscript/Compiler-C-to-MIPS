#pragma once
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include "error_analysis_head.h"
#include "sym_list.h"
#include "mid_code.h"
#include "mips_code.h"

using namespace std;

class compiler {
public:
	compiler();
	void start();

private:
	int line = 1;
	char point;
	int now_point;
	string sym;
	string word;
	string sym1;
	string sym2;
	string word1;
	string word2;
	fstream in;
	fstream out;
	fstream mid_code_stream1;
	fstream mid_code_stream2;
	fstream mips_code_stream;
	vector<string> have_return_func;
	string file_in;
	int index = -1;

	//词法分析
	bool insentive_compare_char(unsigned char a, unsigned char b);
	bool insentive_compare(string a, string b);
	void memorize_two_steps();
	void goesback_two_steps();

	//语法分析
	void getsym();
	void program();
	void analysis_first_letter();
	void var_discription();
	void var_defination();
	void var_def_init();
	void var_def_non_init();
	void main_func();
	void const_discription();
	void const_defination();
	void integer(int* value);
	void non_sign_integer();
	void non_return();
	void have_return(string name, class_type item_class_type, return_type item_return_type);
	void paratable(int* para_num, int this_func_symbol_num);
	void para(int* para_num, int this_func_symbol_num);
	void counpund(string name, string func_end_label);
	void statements(string name, string func_end_label);
	void statement(string name, string func_end_label);
	void return_state(string name, string func_end_label);
	void switch_state(string name, string func_end_label);
	void default_table(string name, string func_end_label);
	void case_table(return_type item_return_type, string expression_value, string switch_end_label, string name, string func_end_label);
	void case_substate(return_type item_return_type, string expression_value, string switch_end_label, string name, string next_case_label, string func_end_label);
	void printf_state(string name, string func_end_label);
	void string_();
	void scanf_state(string name, string func_end_label);
	void if_state(string name, string func_end_label);
	void loop_state(string name, string func_end_label);
	void foodstep();
	void condition(string label, bool flag);
	void expression(return_type* item_return_type, string* expression_value);
	void term(return_type* item_return_type, string* expression_value);
	void factor(return_type* item_return_type, string* expression_value);
	void have_return_use(string name);
	//void non_return_use();
	void value_para(string name);
	void assign_state(string name, string func_end_label);
	return_type constant();

	//错误处理
	void error_collect(error_type error_type, int error_line);
	void error_output();
	void push_back();
	void jump_to_right_sym();
	error error_record[20];
	int error_num = 0;
	bool whether_func_have_return = false;
	return_type now_func_return_type_expected = NOTHING;
	return_type now_func_return_type = NOTHING;
	bool whether_use_func_para_num_right = true;
	bool whether_use_func_para_type_right = true;

	//符号表
	symbol* push_sym_list(string name, class_type item_class_type, return_type item_return_type, int para_num, int line, int value, bool first_use);
	void pop_sym_list();
	void enter_func();
	void change_value(string name, int value);
	bool whether_redefine_name_when_use_IDENFR(string name);
	bool whether_undefine_name(string name);
	bool whether_redefine_name_when_use_FUNC(string name);
	return_type get_return_type(string name);
	symbol* symbol_list[100000];
	symbol** func_table[10000];
	int symbol_num = 0;
	int last_func = 0;
	int begin_func = 0;
	int temp_line;
	int last_word_line = 0;
	int address = 0;
	int globaladdress = 0;
	int func_item_num[10000];
	int func_address[10000];
	int func_num = 0;
	int cur_func = -1;

	//中间代码
	void push_midcode(mid_operation mid_operation, string name1, string name2, string result_name);
	void push_midcode_special(mid_operation operaton, string name1, string name2, string name3, string result_name, int lie);
	void int_to_string(string* s, int value);
	symbol* find_sym_Local(string name);
	symbol* find_sym_Global(string name);
	symbol* func_find(string name);
	bool whether_op_is_num(string op);
	bool whether_op_is_char(string op);
	bool whether_op_is_ret(string op);
	bool whether_op_is_temp(string op);
	void Deal_with_temp(std::string* point);
	void Deal_with_label(std::string* point);
	void printmidcode1();
	void printmidcode2();
	mid_code_struct* mid_codes[1000000];
	int mid_code_num = 0;
	int cur_temp = 0;
	int cur_label = 0;

	//void print_midcode();
	

	//mips代码
	void push_mipscode(mips_code_struct* mips_code);
	void initial_regs();
	void initial_ascii();
	void Deal_with_mips();
	void Deal_with_func(mid_code_struct* code);
	void Deal_with_mul_div(mid_code_struct* code);
	void Deal_with_add_sub(mid_code_struct* code);
	void Deal_with_scanf(mid_code_struct* code);
	void Deal_with_printf(mid_code_struct* code);
	void Deal_with_compare(mid_code_struct* code);
	void Deal_with_label(mid_code_struct* code);
	void Deal_with_goto(mid_code_struct* code);
	void Deal_with_func_return(mid_code_struct* code);
	void Deal_with_real_para(mid_code_struct* code);
	void Deal_with_use_func(mid_code_struct* code);
	void Deal_with_exit(mid_code_struct* code);

	void Deal_with_left_array(mid_code_struct* code);
	void Deal_with_right_array(mid_code_struct* code);
	void Deal_with_left_double_array(mid_code_struct* code);
	void Deal_with_right_double_array(mid_code_struct* code);

	void LW(string rs, int offset, string rt);
	void SW(string rs, int offset, string rt);
	void ADDU_IMM(string rd, string rt, int imm_num);
	void ADDU_REG(string rd, string rs, string rt);
	void SUBU_REG(string rd, string rs, string rt);
	void SLL(string rd, string rt, int imm_num);
	void SRL(string rd, string rt, int imm_num);
	void MUL(string rd, string rs, string rt);
	void DIV(string rs, string label);
	void MFLO(string target);
	void BEQ(string rd, string rs, string rt);
	void BNE(string rd, string rs, string rt);
	void BGEZ(string rs, string label);
	void BGTZ(string rs, string label);
	void BLEZ(string rs, string label);
	void BLTZ(string rs, string label);
	void J(string target);
	void JAL(string target);
	void JR(string target);
	void LI_IMM(string rs, int num);
	void LI_STR(string rs, string label);
	void LA(string rs, string label);
	void LABEL(string label);
	void SYSCALL();
	

	void choose_result_name_reg(string* rd, string* reg);
	void choose_name1_name2_reg(string* rs, string* reg);
	void store_word(string rd, string reg);
	void def_global_value(string name, symbol** sym, bool* global);
	int push_string(string s);
	void print_mipscode();
	string string_table[10000];
	int string_num = 0;
	mips_code_struct* mips_codes[1000000];
	int mips_code_num = 0;
	string regs[14];
	bool enter_first_func = false;
	
	
	//优化
	void optimize_mid();
	void optimize_mips();
	void func_inner_connect();
	void ADD_SUB_optimise();
	void label_optimise();
	void SW_LW_optimise();
	void SW_LW_Cal_optimise();
	void MUL_optimise();
	bool whether_is_pow2(int num);
	int get_log2_index(int num);


	symbol* find_fucking_func(string name);

	symbol* now_func;
};

/*
#pragma once
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include "error_analysis_head.h"
#include "sym_list.h"

using namespace std;

class compiler {
public:
	compiler();
	void start();

private:
	int line = 1;
	char point;
	string sym;
	string word;
	string sym1;
	string sym2;
	string word1;
	string word2;
	fstream in;
	fstream out;
	vector<string> have_return_func;
	int now_point;

	bool insentive_compare_char(unsigned char a, unsigned char b);
	bool insentive_compare(string a, string b);

	void memorize_two_steps();
	void goesback_two_steps();

	void getsym();
	void program();
	void analysis_first_letter();
	void var_discription();
	void var_defination();
	void var_def_init();
	void var_def_non_init();
	void main_func();
	void const_discription();
	void const_defination();
	void integer();
	void non_sign_integer();
	void non_return();
	void have_return(string name, class_type item_class_type, return_type item_return_type);
	void paratable(int *para_num, int this_func_symbol_num);
	void para(int* para_num, int this_func_symbol_num);
	void counpund();
	void statements();
	void statement();
	void return_state();
	void switch_state();
	void default_table();
	void case_table(return_type item_return_type);
	void case_substate(return_type item_return_type);
	return_type constant();
	void printf_state();
	void string_();
	void scanf_state();
	void if_state();
	void loop_state();
	void foodstep();
	void condition();
	void expression(return_type* item_return_type);
	void term(return_type* item_return_type);
	void factor(return_type* item_return_type);
	void have_return_use(string name);
	//void non_return_use();
	void value_para(string name);
	void assign_state();

	error error_record[20];
	int error_num = 0;
	void error_collect(error_type error_type, int error_line);
	void error_output();
	void push_back();
	void jump_to_right_sym();
	bool whether_func_have_return = false;
	return_type now_func_return_type_expected = NOTHING;
	return_type now_func_return_type = NOTHING;
	bool whether_use_func_para_num_right = true;
	bool whether_use_func_para_type_right = true;

	symbol* push_sym_list(string name, class_type item_class_type, return_type item_return_type, int para_num, int line);
	void pop_sym_list();
	void enter_func();
	bool whether_redefine_name_when_use_IDENFR(string name);
	bool whether_undefine_name(string name);
	bool whether_redefine_name_when_use_FUNC(string name);
	return_type get_return_type(string name);
	symbol* symbol_list[100];
	int symbol_num = 0;
	int last_func = 0;
	int begin_func = 0;


	int temp_line;

	int last_word_line = 0;

};
*/