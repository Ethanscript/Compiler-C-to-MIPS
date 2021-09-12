#pragma once
#include "compiler.h"

enum return_type { INT, CHAR, VOID, NOTHING, HAVE_ERROR_COLLECT };
enum class_type { CONST, VAR, ARRAY, FUNC, PARA };

typedef struct {
	std::string name;
	return_type item_return_type;	//
	class_type item_class_type;	//
	int para_num;
	return_type para_list[10];
	int line;
	int value;
	bool first_use;
	int address;
	int reg;
	int has_func;
	bool has_global;
	int position;
	int array_length_hang;
	int array_length_lie;
	
	int array1[1000];
	int array2[1000][1000];

	bool array1_first_use[1000];
	bool array2_first_use[1000][1000];



}symbol;
