#include "compiler.h"
#include "sym_list.h"

symbol* compiler::push_sym_list(string name, class_type item_class_type, return_type item_return_type, int para_num, int line, int value, bool first_use) {
	symbol* new_symbol = new symbol();
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	new_symbol->name = name;
	new_symbol->item_return_type = item_return_type;
	new_symbol->item_class_type = item_class_type;
	new_symbol->para_num = para_num;
	new_symbol->line = line;
	new_symbol->value = value;		//????????????????????????????/有其他含义 para_num ???
	new_symbol->address = address;
	new_symbol->reg = -1;
	new_symbol->first_use = first_use;
	new_symbol->has_func = 0;
	new_symbol->has_global = false;

	new_symbol->array_length_hang = 0;
	new_symbol->array_length_lie = 0;

	if (item_class_type == CONST || item_class_type == VAR || item_class_type == PARA) {
		address = address + 1;
	}
	else if (item_class_type == ARRAY) {
		address = address + value;
	}
	symbol_list[symbol_num] = new_symbol;
	symbol_num = symbol_num + 1;
	return new_symbol;
}

void compiler::change_value(string name, int value) {	// loop range maybe wrong !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	for (int i = symbol_num - 1; i >= 0; i--) {
		if ((*(symbol_list[i])).name == name) {
			(*(symbol_list[i])).value = value;
			return;
		}
	}
}

symbol* compiler::find_sym_Local(string name) {	// loop range maybe wrong !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	symbol* sym = 0;
	for (int i = begin_func; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			sym = symbol_list[i];
			return sym;
		}
	}
	return sym;
}

symbol* compiler::find_sym_Global(string name) {
	symbol* sym = 0;
	for (int i = symbol_num - 1; i >= 0; i--) {
		if ((*(symbol_list[i])).name == name) {
			sym = symbol_list[i];
			return sym;
		}
	}
	return sym;
}

void compiler::pop_sym_list() {
	func_item_num[func_num] = symbol_num - last_func;
	func_table[func_num] = new symbol * [500];
	while (symbol_num > last_func) {	//可能有误
		symbol_num--;
		symbol* sym = symbol_list[symbol_num];
		func_table[func_num][symbol_num - last_func] = sym;
	}
	func_address[func_num] = address;
	symbol_list[symbol_num - 1]->position = func_num;
	address = globaladdress;

	//symbol_num = last_func + 1; /////////////////////////////////////////保留函数名字
	last_func = 0;
	func_num++;
}

void compiler::enter_func() {
	globaladdress = address;
	last_func = symbol_num;
	address = 0;
	begin_func = symbol_num;	/////// 读掉函数名， 为该函数下的同一作用域symbol的起始位置 !!!!symbol_num 是个数，begin_func 是下标，本身就少1
}

bool compiler::whether_redefine_name_when_use_IDENFR(string name) {
	for (int i = begin_func; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			return true;
		}
	}
	return false;
}

bool compiler::whether_undefine_name(string name) {
	for (int i = 0; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			return true;
		}
	}
	return false;
}

bool compiler::whether_redefine_name_when_use_FUNC(string name) {
	for (int i = 0; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			return true;
		}
	}
	return false;
}


return_type compiler::get_return_type(string name) {
	for (int i = symbol_num - 1; i >= 0; i--) {
		if ((*(symbol_list[i])).name == name) {
			return (*(symbol_list[i])).item_return_type;
		}
	}
	return NOTHING;
}

/*
#include "compiler.h"
#include "sym_list.h"

symbol* compiler::push_sym_list(string name, class_type item_class_type, return_type item_return_type, int para_num, int line) {
	symbol* new_symbol = new symbol();
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	new_symbol->name = name;
	new_symbol->item_return_type = item_return_type;
	new_symbol->item_class_type = item_class_type;
	new_symbol->para_num = para_num;
	new_symbol->line = line;
	symbol_list[symbol_num] = new_symbol;
	symbol_num = symbol_num + 1;
	return new_symbol;
}

void compiler::pop_sym_list() {
	symbol_num = last_func + 1; /////////////////////////////////////////保留函数名字
	last_func = 0;
}

void compiler::enter_func() {
	last_func = symbol_num;
	begin_func = symbol_num;	/////// 读掉函数名， 为该函数下的同一作用域symbol的起始位置 !!!!symbol_num 是个数，begin_func 是下标，本身就少1
}

bool compiler::whether_redefine_name_when_use_IDENFR(string name) {
	for (int i = begin_func; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			return true;
		}
	}
	return false;
}

bool compiler::whether_undefine_name(string name) {
	for (int i = 0; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			return true;
		}
	}
	return false;
}

bool compiler::whether_redefine_name_when_use_FUNC(string name) {
	for (int i = 0; i < symbol_num; i++) {
		if ((*(symbol_list[i])).name == name) {
			return true;
		}
	}
	return false;
}


return_type compiler::get_return_type(string name) {
	for (int i = symbol_num - 1; i >= 0; i--) {
		if ((*(symbol_list[i])).name == name) {
			return (*(symbol_list[i])).item_return_type;
		}
	}
}
*/