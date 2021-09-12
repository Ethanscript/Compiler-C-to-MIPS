#pragma once
#include "compiler.h"

enum mips_operation
{
	MIPS_LW, MIPS_SW, MIPS_ADDU_IMM, MIPS_ADDU_REG,
	MIPS_SUBU_REG, MIPS_SLL, MIPS_SRL,
	MIPS_MUL, MIPS_DIV, MIPS_MFLO, MIPS_BEQ, MIPS_BNE,
	MIPS_BGEZ, MIPS_BGTZ, MIPS_BLEZ, MIPS_BLTZ, 
	MIPS_J, MIPS_JAL, MIPS_JR,
	MIPS_LI_IMM, MIPS_LI_STR, MIPS_LA, MIPS_SYSCALL,
	MIPS_LABEL, MIPS_STRING
};

typedef struct {
	mips_operation operation;
	std::string rs;
	std::string rt;
	std::string rd;
	std::string label;
	std::string target;
	std::string string_buff;
	int offset;
	int imm_num;
	int num;

}mips_code_struct;