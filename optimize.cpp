#include "compiler.h"

void compiler::optimize_mid() {
	//func_inner_connect();
	ADD_SUB_optimise();
	label_optimise();
}

void compiler::optimize_mips() {
	
	MUL_optimise();
	SW_LW_optimise();
	SW_LW_Cal_optimise();
}

void compiler::func_inner_connect() {
	;
}

symbol* compiler::find_fucking_func(string name) {
	return 0;
}

void compiler::MUL_optimise() {
	int orignal_mips_num = mips_code_num;
	mips_code_num = 0;
	for (int i = 0; i < orignal_mips_num; i++) {
		mips_code_struct* code1 = mips_codes[i];
		mips_operation operation1 = code1->operation;
		if (operation1 == MIPS_LI_IMM) {
			string rs1 = code1->rs;
			int num1 = code1->num;

			mips_code_struct* code2 = mips_codes[i + 1];
			mips_operation operation2 = code2->operation;
			if (operation2 == MIPS_MUL) {	//x * 4
				string rd2 = code2->rd;
				string rs2 = code2->rs;
				string rt2 = code2->rt;

				if (rs1 == rt2 && whether_is_pow2(num1)) {
					mips_codes[i]->operation = MIPS_SLL;
					mips_codes[i]->rd = rd2;
					mips_codes[i]->rt = rs2;
					mips_codes[i]->imm_num = get_log2_index(num1);
					mips_codes[mips_code_num++] = mips_codes[i];
					i = i + 1;

				}
				else {
					mips_codes[mips_code_num++] = mips_codes[i];
				}
			}
			else if (operation2 == MIPS_LW) {	//4 * x
				string rs2 = code2->rs;
				int offset2 = code2->offset;
				string rt2 = code2->rt;

				mips_code_struct* code3 = mips_codes[i + 2];
				mips_operation operation3 = code3->operation;
				if (operation3 == MIPS_MUL) {
					string rd3 = code3->rd;
					string rs3 = code3->rs;
					string rt3 = code3->rt;

					if (rs1 == rs3 && rs2 == rt3 && whether_is_pow2(num1)) {
						mips_codes[mips_code_num++] = mips_codes[i + 1];
						mips_code_struct* temp_code = new mips_code_struct();
						temp_code->operation = MIPS_SLL;
						temp_code->rd = rd3;
						temp_code->rt = rt3;
						temp_code->imm_num = get_log2_index(num1);
						mips_codes[mips_code_num++] = temp_code;
						i = i + 2;
					}
					else {
						mips_codes[mips_code_num++] = mips_codes[i];
					}
				}
				else {
					mips_codes[mips_code_num++] = mips_codes[i];
				}
			}
			else {
				mips_codes[mips_code_num++] = mips_codes[i];
			}
		}
		else if (operation1 == MIPS_LI_STR) {
			string rs1 = code1->rs;
			string label1 = code1->label;
			int num1 = std::stoi(label1);

			mips_code_struct* code2 = mips_codes[i + 1];
			mips_operation operation2 = code2->operation;
			if (operation2 == MIPS_MUL) {
				string rd2 = code2->rd;
				string rs2 = code2->rs;
				string rt2 = code2->rt;

				if (rs1 == rt2 && whether_is_pow2(num1)) {
					mips_codes[i]->operation = MIPS_SLL;
					mips_codes[i]->rd = rd2;
					mips_codes[i]->rt = rs2;
					mips_codes[i]->imm_num = get_log2_index(num1);
					mips_codes[mips_code_num++] = mips_codes[i];
					i = i + 1;

				}
				else {
					mips_codes[mips_code_num++] = mips_codes[i];
				}
			}
			else if (operation2 == MIPS_LW) {	//4 * x
				string rs2 = code2->rs;
				int offset2 = code2->offset;
				string rt2 = code2->rt;

				mips_code_struct* code3 = mips_codes[i + 2];
				mips_operation operation3 = code3->operation;
				if (operation3 == MIPS_MUL) {
					string rd3 = code3->rd;
					string rs3 = code3->rs;
					string rt3 = code3->rt;

					if (rs1 == rs3 && rs2 == rt3 && whether_is_pow2(num1)) {
						mips_codes[mips_code_num++] = mips_codes[i + 1];
						mips_code_struct* temp_code = new mips_code_struct();
						temp_code->operation = MIPS_SLL;
						temp_code->rd = rd3;
						temp_code->rt = rt3;
						temp_code->imm_num = get_log2_index(num1);
						mips_codes[mips_code_num++] = temp_code;
						i = i + 2;
					}
					else {
						mips_codes[mips_code_num++] = mips_codes[i];
					}
				}
				else {
					mips_codes[mips_code_num++] = mips_codes[i];
				}
			}
			else {
				mips_codes[mips_code_num++] = mips_codes[i];
			}
		}
		else {
			mips_codes[mips_code_num++] = mips_codes[i];
		}
	}
}

void compiler::SW_LW_optimise() {
	int orignal_mips_num = mips_code_num;
	mips_code_num = 0;
	for (int i = 0; i < orignal_mips_num; i++) {
		mips_code_struct* code1 = mips_codes[i];
		mips_operation operation1 = code1->operation;
		if (operation1 == MIPS_SW) {
			string rs1 = code1->rs;
			int offset1 = code1->offset;
			string rt1 = code1->rt;

			mips_code_struct* code2 = mips_codes[i + 1];
			mips_operation operation2 = code2->operation;
			if (operation2 == MIPS_LW) {
				string rs2 = code2->rs;
				int offset2 = code2->offset;
				string rt2 = code2->rt;

				if (rs1 == rs2 && offset1 == offset2 && rt1 == rt2) {	//SW==LW
					mips_codes[mips_code_num++] = mips_codes[i];
					i = i + 1;
				}
				else {
					mips_codes[mips_code_num++] = mips_codes[i];
				}
			}
			else {
				mips_codes[mips_code_num++] = mips_codes[i];
			}
		}
		else {
			mips_codes[mips_code_num++] = mips_codes[i];
		}

	}
}

void compiler::SW_LW_Cal_optimise() {
	int orignal_mips_num = mips_code_num;
	mips_code_num = 0;
	for (int i = 0; i < orignal_mips_num; i++) {
		mips_code_struct* code1 = mips_codes[i];
		mips_operation operation1 = code1->operation;
		if (operation1 == MIPS_SW) {
			string rs1 = code1->rs;
			int offset1 = code1->offset;
			string rt1 = code1->rt;

			mips_code_struct* code2 = mips_codes[i + 1];
			mips_operation operation2 = code2->operation;
			if (operation2 == MIPS_LW) {
				string rs2 = code2->rs;
				int offset2 = code2->offset;
				string rt2 = code2->rt;

				mips_code_struct* code3 = mips_codes[i + 2];
				mips_operation operation3 = code3->operation;
				switch (operation3)
				{
				case MIPS_ADDU_REG:
				case MIPS_SUBU_REG:
				case MIPS_MUL:
				case MIPS_BEQ:
				case MIPS_BNE: {
					string rd3 = code3->rd;
					string rs3 = code3->rs;
					string rt3 = code3->rt;
					if (rt1 == rt2 && offset1 == offset2 && rd3 == rs1) {
						if (rs3 == rs2 || rt3 == rs2) {
							mips_codes[mips_code_num++] = mips_codes[i];
							if (rs3 == rs2) {
								mips_codes[i + 2]->rs = rs1;
							}
							if (rt3 == rs2) {
								mips_codes[i + 2]->rt = rs1;
							}
							mips_codes[mips_code_num++] = mips_codes[i + 2];
							i = i + 2;
						}
						else {
							mips_codes[mips_code_num++] = mips_codes[i];
						}
					}
					else {
						mips_codes[mips_code_num++] = mips_codes[i];
					}
					break;
				}
				case MIPS_ADDU_IMM:
				case MIPS_SLL: {
					string rd3 = code3->rd;
					string rt3 = code3->rt;
					if (rt1 == rt2 && offset1 == offset2 && rd3 == rs1) {
						if (rt3 == rs2) {
							mips_codes[mips_code_num++] = mips_codes[i];
							if (rt3 == rs2) {
								mips_codes[i + 2]->rt = rs1;
							}
							mips_codes[mips_code_num++] = mips_codes[i + 2];
							i = i + 2;
						}
						else {
							mips_codes[mips_code_num++] = mips_codes[i];
						}
					}
					else {
						mips_codes[mips_code_num++] = mips_codes[i];
					}
					break;
				}
				default: {
					mips_codes[mips_code_num++] = mips_codes[i];
					break;
				}
				}
			}
			else {
				mips_codes[mips_code_num++] = mips_codes[i];
			}
		}
		else {
			mips_codes[mips_code_num++] = mips_codes[i];
		}
	}
}

void compiler::ADD_SUB_optimise() {
	int orginal_mid_num = mid_code_num;
	mid_code_num = 0;
	for (int i = 0; i < orginal_mid_num; i++) {
		mid_code_struct* code = mid_codes[i];
		mid_operation operation = code->operation;
		string name1 = code->name1;
		string name2 = code->name2;
		string result_name = code->result_name;
		
		if (operation == ADD_OP) {
			if (name1 != "0" || name2 != result_name) {
				mid_codes[mid_code_num++] = code;
			}
			else if (name2 != "0" || name1 != result_name) {
				mid_codes[mid_code_num++] = code;
			}
		}
		else if (operation == SUB_OP) {
			if (name2 != "0" || name1 != result_name) {
				mid_codes[mid_code_num++] = code;
			}
		}
		else {
			mid_codes[mid_code_num++] = code;
		}
	}
	
}

void compiler::label_optimise() {
	int orginal_mid_num = mid_code_num;
	mid_code_num = 0;
	for (int i = 0; i < orginal_mid_num; i++) {
		mid_code_struct* code = mid_codes[i];
		mid_operation operation = code->operation;
		string result_name = code->result_name;	
		if (operation == GOTO_OP) {
			mid_code_struct* next_one = mid_codes[i + 1];
			if (next_one->operation == LAB_OP ) {
				if (next_one->result_name == code->result_name) {
					mid_codes[mid_code_num++] = next_one;
					i++;
				}
				else {
					mid_codes[mid_code_num++] = code;
				}
			}
			else if (next_one->operation == GOTO_OP) {	//?
				mid_codes[mid_code_num++] = code;
				i++;
			}
			else {
				mid_codes[mid_code_num++] = code;
			}
		}
		else if (operation == LAB_OP) {
			mid_codes[mid_code_num++] = code;
			int j = i + 1;
			while (j < orginal_mid_num) {
				mid_code_struct* next_one = mid_codes[j];
				if (next_one->operation != LAB_OP) {
					break;
				}
				else {
					i++;
					for (int n = 0; n < orginal_mid_num; n++) {
						switch (mid_codes[n]->operation)
						{
						case (GRE_OP):
						case (GEQ_OP):
						case (LEQ_OP):
						case (LSS_OP):
						case (EQL_OP):
						case (NEQ_OP):
						case (GOTO_OP):
						{
							if (mid_codes[n]->result_name == next_one->result_name) {
								mid_codes[n]->result_name = result_name;
								break;
							}
							else {
								break;
							}
						}
						default:
							break;
						}
					}
				}
				j = j + 1;
			}
		}
		else {
			mid_codes[mid_code_num++] = code;
		}
	}
}

bool compiler::whether_is_pow2(int num) {
	return (!(num & (num - 1)));
}

int compiler::get_log2_index(int num) {
	int index = 0;
	while (num > 1)
	{
		num >>= 1;
		index++;
	}
	return index;
}