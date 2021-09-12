#include "compiler.h"

int find_shit_bug = 0;

void compiler::memorize_two_steps() {

	now_point = index;
	word1 = word;
	sym1 = sym;
	getsym();
	word2 = word;
	sym2 = sym;
	getsym();
}

void compiler::goesback_two_steps() {
	sym = sym1;
	word = word1;
	index = now_point;
}

// ������ ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}
void compiler::program() {
	getsym();
	if (sym == "CONSTTK") {
		//out << "CONSTTK " << word << "\n";
		const_discription();
	}
	temp_line = line;
	
	memorize_two_steps();

	while (sym != "LPARENT") {
		goesback_two_steps();
		line = temp_line;
		var_discription();

		temp_line = line;
		memorize_two_steps();
	}

	while (sym2 != "MAINTK") {
		goesback_two_steps();
		line = temp_line;

		if (sym == "VOIDTK") {
			non_return();
		}
		else {
			class_type item_class_type = FUNC;
			return_type item_return_type = NOTHING;
			string name;
			if (sym == "INTTK") {
				//out << "INTTK " << word << "\n";
				item_return_type = INT;
				sym2 = sym;
				getsym();
			}
			else if (sym == "CHARTK") {
				//out << "CHARTK " << word << "\n";
				item_return_type = CHAR;
				sym2 = sym;
				getsym();
			}
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				name = word;
				transform(name.begin(), name.end(), name.begin(), ::tolower);	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				getsym();
			}
			if (sym == "LPARENT") {
				//out << "<����ͷ��>\n";
				//out << "LPARENT (\n";
				have_return_func.push_back(word);
				getsym();
				have_return(name, item_class_type, item_return_type);
			}
		}
		temp_line = line;
		memorize_two_steps();
	}
	goesback_two_steps();
	line = temp_line;

	//out << "VOIDTK " << word << "\n";
	getsym();

	class_type item_class_type = FUNC;
	return_type item_return_type = VOID;
	string name = "main";
	if (whether_redefine_name_when_use_FUNC(name)) {
		error_collect(REDEFINE_NAME_ERROR, line);
	}
	else {
		push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
		now_func = symbol_list[symbol_num - 1];
	}
	
	push_midcode(FUNC_INIT_OP, "", "", "main");
	main_func();
	pop_sym_list();
	push_midcode(EXIT_OP, "", "", "");
	//out << "<������>\n";
	//out << "<����>\n";
}

/*in.seekg(now_point, ios::beg);
	sym = sym1;
	analysis_first_letter();
	now_point = in.tellg();
	sym1 = sym;
	while (sym != "SEMICN") {
		getsym();
		if (sym == )
	}*/

// ������˵���� ::= ���������壾;{���������壾;}
void compiler::var_discription() {

	while (sym == "INTTK" || sym == "CHARTK") {

		int temp_line = line;
		memorize_two_steps();

		if (sym != "LPARENT") {
			goesback_two_steps();
			line = temp_line;
			var_defination();
		}
		else {
			line = temp_line;
			goesback_two_steps();
			break;
		}
	}
	//out << "<����˵��>\n";
}

// ���������壾 ::= �����������޳�ʼ����|���������弰��ʼ����
void compiler::var_defination() {
	temp_line = line;
	now_point = index;
	sym1 = sym;
	bool whether_have_equal = false;
	while (sym != "SEMICN" && last_word_line == line) {		// int a \n int b = 2;
		if (sym == "ASSIGN") {
			whether_have_equal = true;
			goesback_two_steps();	//ʵ����������goesback_one_step
			line = temp_line;
			var_def_init();
			//break;
		}
		else {
			getsym();
		}
	}
	if (!whether_have_equal) {
		goesback_two_steps();	//ʵ����������goesback_one_step
		line = temp_line;
		var_def_non_init();
	}
	//out << "<��������>\n";
	if (sym == "SEMICN") {
		//out << "SEMICN ;\n";
		getsym();
	}
	else {
		error_collect(SEMICN_ERROR, last_word_line);
	}
}

// ���������弰��ʼ���� ::= �����ͱ�ʶ��������ʶ����=��������|�����ͱ�ʶ��������ʶ����'['���޷���������']'='{'��������{,��������}'}'|
// �����ͱ�ʶ��������ʶ����'['���޷���������']''['���޷���������']'='{''{'��������{,��������}'}'{, '{'��������{,��������}'}'}'}'
void compiler::var_def_init() {
	class_type item_class_type = VAR;
	return_type item_return_type = NOTHING;
	string name;
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		sym2 = sym;
		item_return_type = INT;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		sym2 = sym;
		item_return_type = CHAR;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		getsym();
	}
	if (sym == "ASSIGN") {
		//out << "ASSIGN =\n";
		getsym();

		int value = 0;
		bool neg = false;

		//constant
		return_type temp_return_type = NOTHING;
		if (sym == "CHARCON") {
			//out << "CHARCON " << word << "\n";
			value = word[0];
			getsym();
			temp_return_type = CHAR;
		}
		else {
			if (sym == "PLUS") {
				//out << "PLUS +\n";
				getsym();
			}
			else if (sym == "MINU") {
				neg = true;
				//out << "MINU -\n";
				getsym();
			}
			if (sym == "INTCON") {
				//out << "INTCON " << word << "\n";
				if (neg == true) {
					value = (-1) * stoi(word);
				}
				else {
					value = stoi(word);
				}
				getsym();
			}
			//out << "<�޷�������>\n";
			//out << "<����>\n";
			temp_return_type = INT;
		}
		//

		if ( item_return_type != temp_return_type) {
			error_collect(CONST_VALUE_TYPE_ERROR, line);
		}
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line, value, false);
			push_midcode(ADD_OP, to_string(value), "0", name);
		}
	}
	if (sym == "LBRACK") {			////////////////////////////////////////////////////else if
		//out << "LBRACK [\n";
		item_class_type = ARRAY;
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			;
		}

		getsym();

		int hang_expected = 0;
		if (sym == "INTCON") {
			//out << "INTCON " << word << "\n";
			hang_expected = std::stoi(word);
			getsym();
		}
		else {
			error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
			getsym();
		}
		if (sym == "RBRACK") {
			//out << "RBRACK ]\n";
			getsym();
		var_def_init1:
			if (sym == "ASSIGN") {
				//out << "ASSIGN =\n";
				getsym();

				push_sym_list(name, item_class_type, item_return_type, 0, line, hang_expected, true);
				symbol_list[symbol_num - 1]->array_length_lie = hang_expected;	///////////////////////////////////////////////////////////////////�����ʼ���Ĵ���

				if (sym == "LBRACE") {
					//out << "LBRACE {\n";
					getsym();

					int lie_point = 0;
					int volume1 = 0;

					int value = 0;
					return_type temp_return_type = NOTHING;
					if (sym == "CHARCON") {
						//out << "CHARCON " << word << "\n";
						value = word[0];
						getsym();
						temp_return_type = CHAR;
					}
					else {
						bool neg = false;
						if (sym == "PLUS") {
							//out << "PLUS +\n";
							getsym();
						}
						else if (sym == "MINU") {
							//out << "MINU -\n";
							neg = true;
							getsym();
						}
						if (sym == "INTCON") {
							//out << "INTCON " << word << "\n";
							if (neg == true) {
								value = (-1) * stoi(word);
							}
							else {
								value = stoi(word);
							}
							getsym();
						}
						//out << "<�޷�������>\n";
						//out << "<����>\n";
						temp_return_type = INT;
					}
					//out << "<����>\n";

					symbol_list[symbol_num - 1]->array1[lie_point] = value;
					//symbol_list[symbol_num - 1]->array1_first_use[lie_point] = false;
					push_midcode(LEFT_ARRAY_OP, to_string(value), to_string(lie_point), name);

					lie_point++;

					if (item_return_type != temp_return_type) {
						error_collect(CONST_VALUE_TYPE_ERROR, line);
					}
					volume1 = volume1 + 1;
					while (sym == "COMMA") {
						//out << "COMMA ,\n";
						getsym();
						int value = 0;
						return_type temp_return_type = NOTHING;
						if (sym == "CHARCON") {
							//out << "CHARCON " << word << "\n";
							value = word[0];
							getsym();
							temp_return_type = CHAR;
						}
						else {
							bool neg = false;
							if (sym == "PLUS") {
								//out << "PLUS +\n";
								getsym();
							}
							else if (sym == "MINU") {
								//out << "MINU -\n";
								neg = true;
								getsym();
							}
							if (sym == "INTCON") {
								//out << "INTCON " << word << "\n";
								if (neg == true) {
									value = (-1) * stoi(word);
								}
								else {
									value = stoi(word);
								}
								getsym();
							}
							//out << "<�޷�������>\n";
							//out << "<����>\n";
							temp_return_type = INT;
						}
						//out << "<����>\n";
						symbol_list[symbol_num - 1]->array1[lie_point] = value;
						//symbol_list[symbol_num - 1]->array1_first_use[lie_point] = false;
						push_midcode(LEFT_ARRAY_OP, to_string(value), to_string(lie_point), name);

						lie_point++;
						if (item_return_type != temp_return_type) {
							error_collect(CONST_VALUE_TYPE_ERROR, line);
						}
						volume1 = volume1 + 1;


					}
					if (sym == "RBRACE") {
						//out << "RBRACE }\n";
						getsym();
					}
					if (hang_expected != volume1) {
						error_collect(ARRAY_INIT_NUM_ERROR, line);
					}
				}
				else {
					error_collect(ARRAY_INIT_NUM_ERROR, line);
					getsym(); //////////////////////////////////////////////////////////////////////��ά����Ψһ����
				}
			}

			
			if (sym == "LBRACK") {
				//out << "LBRACK [\n";
				int lie_expected = 0;
				getsym();
				if (sym == "INTCON") {
					//out << "INTCON " << word << "\n";
					lie_expected = std::stoi(word);
					getsym();
				}
				else {
					error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
					getsym();
				}

				push_sym_list(name, item_class_type, item_return_type, 0, line, hang_expected * lie_expected, true);
				symbol_list[symbol_num - 1]->array_length_lie = lie_expected;

				if (sym == "RBRACK") {
					//out << "RBRACK ]\n";
					getsym();
				var_def_init2:
					if (sym == "ASSIGN") {
						//out << "ASSIGN =\n";
						getsym();
						if (sym == "LBRACE") {
							//out << "LBRACE {\n";
							getsym();
							
							if (sym == "LBRACE") {
								//out << "LBRACE {\n";
								int hang_volume = 0;
								hang_volume = hang_volume + 1;
								getsym();
								int lie_volume = 0;

								/////////////////////////// <summary>

								int value = 0;
								return_type temp_return_type = NOTHING;
								if (sym == "CHARCON") {
									//out << "CHARCON " << word << "\n";
									value = word[0];
									getsym();
									temp_return_type = CHAR;
								}
								else {
									bool neg = false;
									if (sym == "PLUS") {
										//out << "PLUS +\n";
										getsym();
									}
									else if (sym == "MINU") {
										//out << "MINU -\n";
										neg = true;
										getsym();
									}
									if (sym == "INTCON") {
										//out << "INTCON " << word << "\n";
										if (neg == true) {
											value = (-1) * stoi(word);
										}
										else {
											value = stoi(word);
										}
										getsym();
									}
									//out << "<�޷�������>\n";
									//out << "<����>\n";
									temp_return_type = INT;
								}
								//out << "<����>\n";

								symbol_list[symbol_num - 1]->array2[hang_volume - 1][lie_volume] = value;
								//symbol_list[symbol_num - 1]->array2_first_use[hang_volume - 1][lie_volume] = false;

								push_midcode_special(LEFT_DOUBLE_ARRAY_OP, to_string(value), 
									to_string(hang_volume - 1), to_string(lie_volume), name, lie_expected);

								if (item_return_type != temp_return_type) {
									error_collect(CONST_VALUE_TYPE_ERROR, line);
								}

								/////////////////////////// </summary>
								lie_volume = lie_volume + 1;



								while (sym == "COMMA") {
									//out << "COMMA ,\n";
									getsym();
									
									///////////////////////// <summary>

									int value = 0;
									return_type temp_return_type = NOTHING;
									if (sym == "CHARCON") {
										//out << "CHARCON " << word << "\n";
										value = word[0];
										getsym();
										temp_return_type = CHAR;
									}
									else {
										bool neg = false;
										if (sym == "PLUS") {
											//out << "PLUS +\n";
											getsym();
										}
										else if (sym == "MINU") {
											//out << "MINU -\n";
											neg = true;
											getsym();
										}
										if (sym == "INTCON") {
											//out << "INTCON " << word << "\n";
											if (neg == true) {
												value = (-1) * stoi(word);
											}
											else {
												value = stoi(word);
											}
											getsym();
										}
										//out << "<�޷�������>\n";
										//out << "<����>\n";
										temp_return_type = INT;
									}
									//out << "<����>\n";

									symbol_list[symbol_num - 1]->array2[hang_volume - 1][lie_volume] = value;
									//symbol_list[symbol_num - 1]->array2_first_use[hang_volume - 1][lie_volume] = false;

									push_midcode_special(LEFT_DOUBLE_ARRAY_OP, to_string(value),
										to_string(hang_volume - 1), to_string(lie_volume), name, lie_expected);

									if (item_return_type != temp_return_type) {
										error_collect(CONST_VALUE_TYPE_ERROR, line);
									}

									/////////////////////// </summary>
									lie_volume = lie_volume + 1;
								}
								if (sym == "RBRACE") {
									//out << "RBRACE }\n";
									if (lie_expected != lie_volume) {
										error_collect(ARRAY_INIT_NUM_ERROR, line);
									}
									lie_volume = 0;
									getsym();

									while (sym == "COMMA") {	
										//out << "COMMA ,\n";
										hang_volume = hang_volume + 1;
										getsym();
										if (sym == "LBRACE") {
											//out << "LBRACE {\n";
											lie_volume = 0;
											getsym();
											///////////////////// <summary>

											int value = 0;
											return_type temp_return_type = NOTHING;
											if (sym == "CHARCON") {
												//out << "CHARCON " << word << "\n";
												value = word[0];
												getsym();
												temp_return_type = CHAR;
											}
											else {
												bool neg = false;
												if (sym == "PLUS") {
													//out << "PLUS +\n";
													getsym();
												}
												else if (sym == "MINU") {
													//out << "MINU -\n";
													neg = true;
													getsym();
												}
												if (sym == "INTCON") {
													//out << "INTCON " << word << "\n";
													if (neg == true) {
														value = (-1) * stoi(word);
													}
													else {
														value = stoi(word);
													}
													getsym();
												}
												//out << "<�޷�������>\n";
												//out << "<����>\n";
												temp_return_type = INT;
											}
											//out << "<����>\n";

											symbol_list[symbol_num - 1]->array2[hang_volume - 1][lie_volume] = value;
											//symbol_list[symbol_num - 1]->array2_first_use[hang_volume - 1][lie_volume] = false;
											push_midcode_special(LEFT_DOUBLE_ARRAY_OP, to_string(value),
												to_string(hang_volume - 1), to_string(lie_volume), name, lie_expected);
											
											if (item_return_type != temp_return_type) {
												error_collect(CONST_VALUE_TYPE_ERROR, line);
											}

											///////////////////// </summary>
											lie_volume = lie_volume + 1;
											while (sym == "COMMA") {
												//out << "COMMA ,\n";
												getsym();
												/////////////////////////// <summary>

												int value = 0;
												return_type temp_return_type = NOTHING;
												if (sym == "CHARCON") {
													//out << "CHARCON " << word << "\n";
													value = word[0];
													getsym();
													temp_return_type = CHAR;
												}
												else {
													bool neg = false;
													if (sym == "PLUS") {
														//out << "PLUS +\n";
														getsym();
													}
													else if (sym == "MINU") {
														//out << "MINU -\n";
														neg = true;
														getsym();
													}
													if (sym == "INTCON") {
														//out << "INTCON " << word << "\n";
														if (neg == true) {
															value = (-1) * stoi(word);
														}
														else {
															value = stoi(word);
														}
														getsym();
													}
													//out << "<�޷�������>\n";
													//out << "<����>\n";
													temp_return_type = INT;
												}
												//out << "<����>\n";

												symbol_list[symbol_num - 1]->array2[hang_volume - 1][lie_volume] = value;
												//symbol_list[symbol_num - 1]->array2_first_use[hang_volume - 1][lie_volume] = false;
												push_midcode_special(LEFT_DOUBLE_ARRAY_OP, to_string(value),
													to_string(hang_volume - 1), to_string(lie_volume), name, lie_expected);

												if (item_return_type != temp_return_type) {
													error_collect(CONST_VALUE_TYPE_ERROR, line);
												}
												///////////////////// </summary>

												lie_volume = lie_volume + 1;
												if (sym == "RBRACE") {
													//out << "RBRACE }\n";
													if (lie_expected != lie_volume) {
														error_collect(ARRAY_INIT_NUM_ERROR, line);
													}
													lie_volume = 0;
													getsym();	
													break;
												}
											}
										}
									}
									if (hang_expected != hang_volume){
										error_collect(ARRAY_INIT_NUM_ERROR, line);			////////////////////////////////////���治��getsym()
									}
									if (sym == "RBRACE") {
										//out << "RBRACE }\n";
										getsym();
									}
								}
							}
							else {
								error_collect(ARRAY_INIT_NUM_ERROR, line);
								for (int i = 0; i <= lie_expected; i++) {
									getsym();							/////////////////////////���� ��2}�� , �Ƶ���һ��getsym
								}
							}
						}
						else {
							error_collect(ARRAY_INIT_NUM_ERROR, line);
							getsym();								////////////////////////������һ������
						}
					}
				}
				else {
					error_collect(RBRACK_ERROR, last_word_line);
					goto var_def_init2;
				}
			}
		}
		else {
			error_collect(RBRACK_ERROR, last_word_line);
			goto var_def_init1;
		}
	}
	//out << "<�������弰��ʼ��>\n";
}

//�����������޳�ʼ����  ::= �����ͱ�ʶ����(����ʶ����|����ʶ����'['���޷���������']'|����ʶ����'['���޷���������']''['���޷���������']')
//{,(����ʶ����|����ʶ����'['���޷���������']'|����ʶ����'['���޷���������']''['���޷���������']' )}
void compiler::var_def_non_init() {
	class_type item_class_type = VAR;
	return_type item_return_type = NOTHING;
	string name;
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		sym2 = sym;
		item_return_type = INT;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		sym2 = sym;
		item_return_type = CHAR;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		getsym();
	}

	if (sym == "LBRACK") {
		//out << "LBRACK [\n";
		item_class_type = ARRAY;
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			;
		}

		getsym();
		
		int array_index_1 = 0;
		int array_index_2 = 0;

		//�±�
		if (sym == "INTCON") {
			//out << "INTCON " << word << "\n";
			array_index_1 = stoi(word);
			getsym();
		}
		else {
			error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
			getsym();
		}

		if (sym == "RBRACK") {
			//out << "RBRACK ]\n";
			getsym();
		var_def_non_init1:
			if (sym == "LBRACK") {
				//out << "LBRACK [\n";
				getsym();

				//�±�
				if (sym == "INTCON") {
					//out << "INTCON " << word << "\n";
					array_index_2 = stoi(word);
					getsym();
				}
				else {
					error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
					getsym();
				}

				push_sym_list(name, item_class_type, item_return_type, 0, line, array_index_1 * array_index_2, true);
				symbol_list[symbol_num - 1]->array_length_lie = array_index_2;

				if (sym == "RBRACK") {
					//out << "RBRACK ]\n";
					getsym();
				}
				else {
					error_collect(RBRACK_ERROR, last_word_line);
				}
				symbol_list[symbol_num - 1]->array_length_lie = array_index_2;
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line, array_index_1, true);
				symbol_list[symbol_num - 1]->array_length_lie = array_index_1;

			}
		}
		else {
			error_collect(RBRACK_ERROR, last_word_line);
			goto var_def_non_init1;
		}
	}
	else {
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
		}
	}

	while (sym == "COMMA") {

		//out << "COMMA ,\n";
		getsym();

		if (sym == "IDENFR") {
			//out << "IDENFR " << word << "\n";
			name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			getsym();
		}
		int array_index_1 = 0;
		int array_index_2 = 0;

		if (sym == "LBRACK") {
			//out << "LBRACK [\n";
			item_class_type = ARRAY;
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				;
			}
			getsym();

			//�±�
			if (sym == "INTCON") {
				//out << "INTCON " << word << "\n";
				array_index_1 = stoi(word);
				getsym();
			}
			else {
				error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
				getsym();
			}

			if (sym == "RBRACK") {
				//out << "RBRACK ]\n";
				getsym();
			var_def_non_init2:
				if (sym == "LBRACK") {
					//out << "LBRACK [\n";
					getsym();
					
					//�±�
					if (sym == "INTCON") {
						//out << "INTCON " << word << "\n";
						array_index_2 = stoi(word);
						getsym();
					}
					else {
						error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
						getsym();
					}
					push_sym_list(name, item_class_type, item_return_type, 0, line, array_index_1 * array_index_2, true);
					symbol_list[symbol_num - 1]->array_length_lie = array_index_2;
					
					if (sym == "RBRACK") {
						//out << "RBRACK ]\n";
						getsym();
					}
					else {
						error_collect(RBRACK_ERROR, last_word_line);
					}
				}
				else {
					push_sym_list(name, item_class_type, item_return_type, 0, line, array_index_1, true);
					symbol_list[symbol_num - 1]->array_length_lie = array_index_1;

				}
			}
			else {
				error_collect(RBRACK_ERROR, last_word_line);
				goto var_def_non_init2;
			}
		}
		else {
			item_class_type = VAR;
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
			}
		}
	}
	//out << "<���������޳�ʼ��>\n";
}

// ���������� ::= void main��(����)�� ��{����������䣾��}��
void compiler::main_func() {
	enter_func();
	if (sym == "MAINTK") {
		//out << "MAINTK " << word << "\n";
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			main_func1:
				if (sym == "LBRACE") {
					//out << "LBRACE {\n";
					getsym();

					now_func_return_type_expected = VOID;
					counpund("main", "");

					if (sym == "RBRACE") {
						//out << "RBRACE }\n";
						getsym();
					}
				}
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
				goto main_func1;
			}
		}
	}
}

// ������˵���� ::=  const���������壾;{ const���������壾;} 
void compiler::const_discription() {
	do {
		getsym();
		const_defination();
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		const_discription1:
			if (sym == "CONSTTK") {
				//out << "CONSTTK " << word << "\n";
			}
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
			goto const_discription1;
		}
	} while (sym == "CONSTTK");
	//out << "<����˵��>\n";
}

// ���������壾 ::= int����ʶ��������������{,����ʶ��������������}  | char����ʶ���������ַ���{ ,����ʶ���������ַ��� }
void compiler::const_defination() {
	class_type item_class_type = CONST;
	if (sym == "INTTK") {
		return_type item_return_type = INT;
		//out << "INTTK " << word << "\n";
		do {
			getsym();
			string name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
			}
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				getsym();

				int value = 0;
				bool neg = false;

				if (sym == "ASSIGN") {
					//out << "ASSIGN =\n";
					getsym();
				}
				if (sym == "PLUS") {
					//out << "PLUS +\n";
					getsym();
				}
				else if (sym == "MINU") {
					//out << "MINU -\n";
					getsym();
					neg = true;
				}
				if (sym == "INTCON") {
					//out << "INTCON " << word << "\n";
					if (neg == true) {
						value = (-1) * stoi(word);
					}
					else {
						value = stoi(word);
					}
					change_value(name, value);
					//push_midcode(ADD_OP, to_string(value), "0", name);
					getsym();
					//out << "<�޷�������>\n";
				}

			}
			if (sym == "COMMA") {
				//out << "COMMA ,\n";
			}
		} while (sym == "COMMA");
	}
	else if (sym == "CHARTK") {
		return_type item_return_type = CHAR;
		//out << "CHARTK " << word << "\n";
		do {
			getsym();
			string name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
			}
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				getsym();
				if (sym == "ASSIGN") {
					//out << "ASSIGN =\n";
					getsym();
				}
				if (sym == "CHARCON") {
					int value = 0;
					value = word[0];
					change_value(name, value);
					//push_midcode(ADD_OP, to_string(value), "0", name);
					//out << "CHARCON " << word << "\n";
					getsym();
				}
			}
			if (sym == "COMMA") {
				//out << "COMMA ,\n";
			}
		} while (sym == "COMMA");
	}
	//out << "<��������>\n";
}

// �������� ::= �ۣ������ݣ��޷���������
void compiler::integer(int* value) {
	bool neg = false;
	if (sym == "PLUS") {
		//out << "PLUS +\n";
		getsym();
	}
	else if (sym == "MINU") {
		//out << "MINU -\n";
		neg = true;
		getsym();
	}
	if (sym == "INTCON") {
		//out << "INTCON " << word << "\n";
		if (neg == true) {
			*value = (-1) * stoi(word);
		}
		else {
			*value = stoi(word);
		}
		getsym();
	}

	//out << "<�޷�������>\n";
	//out << "<����>\n";
}

// ���޷���ֵ�������壾 ::= void����ʶ����'('��������')''{'��������䣾'}'
void compiler::non_return() {

	bool whether_add_func = false;

	//out << "VOIDTK " << word << "\n";
	class_type item_class_type = FUNC;
	return_type item_return_type = VOID;
	string name;
	getsym();
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		getsym();

		push_midcode(FUNC_INIT_OP, "", "", name);

		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
		}

		if (whether_redefine_name_when_use_FUNC(name)) {
			whether_add_func = false;
			error_collect(REDEFINE_NAME_ERROR, line);
			enter_func();
			int para_num = 0;
			paratable(&para_num, -1);
		}
		else {
			whether_add_func = true;
			int this_func_symbol_num = symbol_num;
			push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
			now_func = symbol_list[symbol_num - 1];

			enter_func();
			int para_num = 0;
			paratable(&para_num, this_func_symbol_num);
			(*(symbol_list[this_func_symbol_num])).para_num = para_num;
		}

		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}
		if (sym == "LBRACE") {
			//out << "LBRACE {\n";
			getsym();
		}

		std::stringstream func_end;
		func_end << name << "_end";
		string func_end_label = func_end.str();

		now_func_return_type_expected = VOID;
		counpund(name, func_end_label);

		if (sym == "RBRACE") {
			//out << "RBRACE }\n";
			getsym();
		}

		
		push_midcode(LAB_OP, "", "", func_end_label);
		push_midcode(FUNC_RETURN_OP, "", "", name);
		

		if (!whether_add_func) {
			symbol_num = last_func; 
			last_func = 0;
		}
		else {
			pop_sym_list();
		}
	}
	//out << "<�޷���ֵ��������>\n";
}

// ���з���ֵ�������壾 ::= ������ͷ����'('��������')' '{'��������䣾'}'
void compiler::have_return(string name, class_type item_class_type, return_type item_return_type) {

	push_midcode(FUNC_INIT_OP, "", "", name);

	bool whether_add_func = false;
	
	if (whether_redefine_name_when_use_FUNC(name)) {
		whether_add_func = false;
		error_collect(REDEFINE_NAME_ERROR, line);
		enter_func();
		int para_num = 0;
		paratable(&para_num, -1);
	}
	else {
		whether_add_func = true;
		int this_func_symbol_num = symbol_num;
		push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);	///////////////////////////////////////////////
		now_func = symbol_list[symbol_num - 1];

		enter_func();
		int para_num = 0;
		paratable(&para_num, this_func_symbol_num);
		(*(symbol_list[this_func_symbol_num])).para_num = para_num;
	}

	now_func_return_type_expected = item_return_type;
	std::stringstream func_end;
	func_end << name << "_end";
	string func_end_label = func_end.str();

	if (sym == "RPARENT") {
		//out << "RPARENT )\n";
		getsym();
	have_return1:
		if (sym == "LBRACE") {
			//out << "LBRACE {\n";
			getsym();

			now_func_return_type = NOTHING;
			counpund(name, func_end_label);
			if (now_func_return_type == NOTHING) {
				error_collect(HAVE_RETURN_FUNC_ERROR, line);
			}

			if (sym == "RBRACE") {
				//out << "RBRACE }\n";
				getsym();
			}
		}
	}
	else {
		error_collect(RPARENT_ERROR, last_word_line);
		goto have_return1;
	}

	push_midcode(LAB_OP, "", "", func_end_label);
	push_midcode(FUNC_RETURN_OP, "", "", name);

	if (!whether_add_func) {
		symbol_num = last_func;
		last_func = 0;
	}
	else {
		pop_sym_list();
	}
	//out << "<�з���ֵ��������>\n";
}

// �������� ::= �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
void compiler::paratable(int* para_num, int this_func_symbol_num) {
	para(para_num, this_func_symbol_num);
	while (sym == "COMMA") {
		//out << "COMMA ,\n";
		getsym();
		para(para_num, this_func_symbol_num);
	}
	//out << "<������>\n";
}

void compiler::para(int* para_num, int this_func_symbol_num) {
	class_type item_class_type = PARA;
	return_type item_return_type = NOTHING;
	string name;
	transform(name.begin(), name.end(), name.begin(), ::tolower);	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		item_return_type = INT;
		(*symbol_list[this_func_symbol_num]).para_list[*para_num] = INT;
		*para_num = *para_num + 1;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		item_return_type = CHAR;
		(*symbol_list[this_func_symbol_num]).para_list[*para_num] = CHAR;
		*para_num = *para_num + 1;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line, INT32_MAX, true);
		}
		getsym();
	}
}

// ��������䣾 ::= �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У� 
void compiler::counpund(string name, string func_end_label) {
	if (sym == "CONSTTK") {
		//out << "CONSTTK " << word << "\n";
		const_discription();
	}
	if (sym == "INTTK" || sym == "CHARTK") {
		var_discription();
	}
	statements(name, func_end_label);
	//out << "<�������>\n";
}

// ������У� ::= ������䣾��
void compiler::statements(string name, string func_end_label) {
	while (sym == "WHILETK" || sym == "FORTK" || sym == "IFTK" || sym == "IDENFR"
		|| sym == "SCANFTK" || sym == "PRINTFTK" || sym == "SWITCHTK"
		|| sym == "SEMICN" || sym == "RETURNTK" || sym == "LBRACE") {
		statement(name, func_end_label);
	}
	//out << "<�����>\n";
}

// ����䣾 ::= ��ѭ����䣾����������䣾| ���з���ֵ����������䣾;  |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;���������䣾�����գ�;|��������䣾; | '{'������У�'}' 
void compiler::statement(string name, string func_end_label) {					// switch
	if (sym == "WHILETK") {
		loop_state(name, func_end_label);
	}
	else if (sym == "FORTK") {
		loop_state(name, func_end_label);
	}
	else if (sym == "IFTK") {
		if_state(name, func_end_label);
	}
	else if (sym == "IDENFR") {
		bool whether_return = false;
		//out << "IDENFR " << word << "\n";
		string name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (!whether_undefine_name(name)) {
			error_collect(UNDEFIEN_NAME_ERROR, line);
		}
		if (find(have_return_func.begin(), have_return_func.end(), word) != have_return_func.end()) {
			whether_return = true;
		}
		if (!find_sym_Local(name)) {										//  ��Χ���ܴ���
			now_func->has_global = true;
		}
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "SEMICN" || sym == "COMMA") {	// ���� c(;;;;; ��ɵ�ƴ�
				error_collect(RPARENT_ERROR, last_word_line);
			}
			else {
				value_para(name);
				push_midcode(USE_FUNC_OP, "", "", name);
				now_func->has_func = now_func->has_func + 1;
				
				if (sym == "RPARENT") {
					//out << "RPARENT )\n";
					getsym();
				statement1:
					if (whether_return) {
						//have_return_use();		////////////////////////////////////////////////////////
						//out << "<�з���ֵ�����������>\n";
					}
					else {
						//non_return_use();		/////////////////////////////////////////////////////////
						//out << "<�޷���ֵ�����������>\n";
					}
				}
				else {
					error_collect(RPARENT_ERROR, last_word_line);
					goto statement1;
				}
				if (sym == "SEMICN") {
					//out << "SEMICN ;\n";
					getsym();
				}
				else {
					error_collect(SEMICN_ERROR, last_word_line);
				}
			}
		}
		else if (sym == "ASSIGN" || sym == "LBRACK") {
			for (int i = 0; i < symbol_num; i++) {
				if ((*(symbol_list[i])).name == name) {
					if ((*(symbol_list[i])).item_class_type == CONST) {
						error_collect(CONST_VALUE_CHANGE_ERROR, line);
						break;
					}
				}
			}
			assign_state(name, func_end_label);
			/*
			for (int i = 0; i < symbol_num; i++) {
				if ((*(symbol_list[i])).name == name) {
					(*(symbol_list[i])).first_use = true;				/////////////////////// assign ����, ������֮ǰ��ֵ��
				}
			}
			*/
		}

		else if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {											//////////////////////////
			error_collect(SEMICN_ERROR, last_word_line);
		}



	}
	else if (sym == "SCANFTK") {
		scanf_state(name, func_end_label);
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
		}
	}
	else if (sym == "PRINTFTK") {
		printf_state(name, func_end_label);
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
		}
	}
	else if (sym == "SWITCHTK") {
		switch_state(name, func_end_label);
	}
	else if (sym == "SEMICN") {
		//out << "SEMICN ;\n";
		getsym();

	}
	else if (sym == "RETURNTK") {
		return_state(name, func_end_label);
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
		}
	}
	else if (sym == "LBRACE") {
		//out << "LBRACE {\n";
		getsym();
		statements(name, func_end_label);
		if (sym == "RBRACE") {
			//out << "RBRACE }\n";
			getsym();
		}
	}
	else {
		error_collect(SEMICN_ERROR, last_word_line);	/////////////////////////////////
	}
	//out << "<���>\n";
}

// ����ֵ��䣾 ::= ����ʶ�����������ʽ��|����ʶ����'['�����ʽ��']'=�����ʽ��|����ʶ����'['�����ʽ��']''['�����ʽ��']' =�����ʽ��
void compiler::assign_state(string name, string func_end_label) {
	if (sym == "ASSIGN") {
		//out << "ASSIGN =\n";
		getsym();
		return_type item_return_type = NOTHING;
		string expression_value = "";
		expression(&item_return_type, &expression_value);
		if (whether_op_is_temp(expression_value)) {				// ת������Ĵ��� nb �迴��
			mid_code_struct* code = mid_codes[mid_code_num - 1];
			code->result_name = name;
		}
		else {
			push_midcode(ADD_OP, expression_value, "0", name);
		}
	}
	else if (sym == "LBRACK") {
		//out << "LBRACK [\n";
		getsym();
		return_type item_return_type = NOTHING;

		string tmp_expression_value_left_1 = "";
		expression(&item_return_type, &tmp_expression_value_left_1);


		if (item_return_type != INT) {
			error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
		}
		if (sym == "RBRACK") {
			//out << "RBRACK ]\n";
			getsym();
		assign_state1:
			if (sym == "ASSIGN") {
				//out << "ASSIGN =\n";
				getsym();
				return_type item_return_type = NOTHING;
				
				string tmp_expression_value_right = "";
				expression(&item_return_type, &tmp_expression_value_right);
				push_midcode(LEFT_ARRAY_OP, tmp_expression_value_right, tmp_expression_value_left_1, name);

				/*
				for (int i = 0; i < symbol_num; i++) {			//////////////��ʼλ�ÿ��ܴ���
					if ((*(symbol_list[i])).name == name) {
						(*(symbol_list[i])).first_use = true;
					}
				}
				*/
			}	
			if (sym == "LBRACK") {
				//out << "LBRACK [\n";
				getsym();
				return_type item_return_type = NOTHING;

				string tmp_expression_value_left_2 = "";
				expression(&item_return_type, &tmp_expression_value_left_2);
				if (item_return_type != INT) {
					error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
				}
				if (sym == "RBRACK") {
					//out << "RBRACK ]\n";
					getsym();
				assign_state2:
					if (sym == "ASSIGN") {
						//out << "ASSIGN =\n";
						getsym();
						return_type item_return_type = NOTHING;

						string tmp_expression_value_right = "";
						expression(&item_return_type, &tmp_expression_value_right);

						int lie = 0;
						for (int i = 0; i < symbol_num; i++) {			//////////////��ʼλ�ÿ��ܴ���
							if ((*(symbol_list[i])).name == name) {
								lie = (*(symbol_list[i])).array_length_lie;
							}
						}
						push_midcode_special(LEFT_DOUBLE_ARRAY_OP, tmp_expression_value_right,
							tmp_expression_value_left_1, tmp_expression_value_left_2, name, lie);

						/*
						for (int i = 0; i < symbol_num; i++) {			//////////////��ʼλ�ÿ��ܴ���
							if ((*(symbol_list[i])).name == name) {
								(*(symbol_list[i])).first_use = true;
							}
						}
						*/
					}
				}
				else {
					error_collect(RBRACK_ERROR, last_word_line);
					goto assign_state2;
				}
			}
		}
		else {
			error_collect(RBRACK_ERROR, last_word_line);
			goto assign_state1;
		}
		

	}
	//out << "<��ֵ���>\n";
	if (sym == "SEMICN") {
		//out << "SEMICN ;\n";
		getsym();
	}
	else {
		error_collect(SEMICN_ERROR, last_word_line);
	}

}

// ��������䣾 ::=  return['('�����ʽ��')']   
void compiler::return_state(string name, string func_end_label) {
	if (sym == "RETURNTK") {
		//out << "RETURNTK " << word << "\n";
		getsym();
	}
	if (sym == "LPARENT") {
		if (now_func_return_type_expected == VOID) {
			error_collect(NON_RETURN_FUNC_ERROR, line);
		}
		//out << "LPARENT (\n";
		getsym();
		string tmp_expression_value = "";

		if (sym != "RPARENT") {
			return_type item_return_type = NOTHING;
			expression(&item_return_type, &tmp_expression_value);
			now_func_return_type = item_return_type;
		}
		else {
			now_func_return_type = NOTHING;
		}
		if ((now_func_return_type_expected != now_func_return_type)&& (now_func_return_type_expected == INT || now_func_return_type_expected == CHAR)) {
			error_collect(HAVE_RETURN_FUNC_ERROR, line);
			now_func_return_type = HAVE_ERROR_COLLECT;
		}


		if (whether_op_is_temp(tmp_expression_value)) {
			mid_codes[mid_code_num - 1]->result_name = "#RET";
		}
		else {
			push_midcode(ADD_OP, tmp_expression_value, "0", "#RET");
		}


		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}
	}
	else {
		if (now_func_return_type_expected == INT || now_func_return_type_expected == CHAR) {
			error_collect(HAVE_RETURN_FUNC_ERROR, line);
			now_func_return_type = HAVE_ERROR_COLLECT;
		}
	}

	if (name == "main") {
		push_midcode(EXIT_OP, "", "", "");
	}
	else {
		push_midcode(GOTO_OP, "", "", func_end_label);
	}


	//out << "<�������>\n";
}

// �������䣾 ::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ����}��  
void compiler::switch_state(string name, string func_end_label) {
	//out << "SWITCHTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		return_type item_return_type = NOTHING;

		string tmp_expression_value = "";
		expression(&item_return_type, &tmp_expression_value);
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}

		if (sym == "LBRACE") {
			//out << "LBRACE {\n";
			getsym();

			string switch_end_label = "";
			Deal_with_label(&switch_end_label);

			case_table(item_return_type, tmp_expression_value, switch_end_label, name, func_end_label);
			default_table(name, func_end_label);

			push_midcode(LAB_OP, "", "", switch_end_label);
		}
		if (sym == "RBRACE") {
			//out << "RBRACE }\n";
			getsym();
		}

	}
	
	//out << "<������>\n";
}

// ��ȱʡ�� ::=  default :����䣾  
void compiler::default_table(string name, string func_end_label) {
	if (sym == "DEFAULTTK") {
		//out << "DEFAULTTK " << word << "\n";
		getsym();
	}
	else {
		error_collect(DEFAULT_DISMISS_ERROR, line);
		goto default_table1;
	}
	if (sym == "COLON") {
		//out << "COLON :\n";
		getsym();
	}
	statement(name, func_end_label);
	//out << "<ȱʡ>\n";
default_table1:
	;
}

// ������� ::=  ���������䣾{���������䣾}
void compiler::case_table(return_type item_return_type, string expression_value,string switch_end_label, string name, string func_end_label) {
	while (sym == "CASETK") {
		string next_case_label = "";
		Deal_with_label(&next_case_label);
		case_substate(item_return_type, expression_value, switch_end_label, name, next_case_label, func_end_label);
		push_midcode(LAB_OP, "", "", next_case_label);
	}
	//out << "<�����>\n";
}

// ���������䣾 ::=  case��������������䣾  
void compiler::case_substate(return_type item_return_type, string expression_value, string switch_end_label, string name, string next_case_label , string func_end_label) {
	if (sym == "CASETK") {
		//out << "CASETK " << word << "\n";
		getsym();
	
		int value = 0;
		return_type temp_return_type = NOTHING;
		if (sym == "CHARCON") {
			//out << "CHARCON " << word << "\n";
			value = word[0];
			getsym();
			temp_return_type = CHAR;
		}
		else {
			bool neg = false;
			if (sym == "PLUS") {
				//out << "PLUS +\n";
				getsym();
			}
			else if (sym == "MINU") {
				//out << "MINU -\n";
				neg = true;
				getsym();
			}
			if (sym == "INTCON") {
				//out << "INTCON " << word << "\n";
				if (neg == true) {
					value = (-1) * stoi(word);
				}
				else {
					value = stoi(word);
				}
				getsym();
			}
			//out << "<�޷�������>\n";
			//out << "<����>\n";
			temp_return_type = INT;
		}
		//out << "<����>\n";

		push_midcode(NEQ_OP, expression_value, to_string(value), next_case_label);

		if (sym == "COLON") {
			//out << "COLON :\n";
			getsym();
			statement(name, func_end_label);
		}
		push_midcode(GOTO_OP, "", "", switch_end_label);
	}
	//out << "<��������>\n";
}

// �������� ::=  ��������|���ַ���
return_type compiler::constant() {
	if (sym == "CHARCON") {
		//out << "CHARCON " << word << "\n";
		getsym();
		return CHAR;
	}
	else {
		int value = 0;
		integer(&value);
		return INT;
	}
	//out << "<����>\n";
}

// ��д��䣾 ::= printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'| printf '('�����ʽ��')' 
void compiler::printf_state(string name, string func_end_label) {
	string* s = 0;
	//out << "PRINTFTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		if (sym == "STRCON") {
			//out << "STRCON " << word << "\n";

			int value = push_string(word);
			s = new std::string();
			int_to_string(s, value);
			string_();

			if (sym == "COMMA") {
				//out << "COMMA ,\n";
				getsym();
				return_type item_return_type = NOTHING;

				string tmp_expression_value = "";
				expression(&item_return_type, &tmp_expression_value);
				if (s) {
					if (item_return_type == INT) {
						push_midcode(PRINTF_OP, *s, "0", *s);
						push_midcode(PRINTF_OP, "-1", "int", tmp_expression_value);
					}
					else if (item_return_type == CHAR) {
						push_midcode(PRINTF_OP, *s, "0", *s);
						push_midcode(PRINTF_OP, "-1", "char", tmp_expression_value);
					}
				}
				else {
					if (item_return_type == INT) {
						push_midcode(PRINTF_OP, "-1", "int", tmp_expression_value);
					}
					else if (item_return_type == CHAR) {
						push_midcode(PRINTF_OP, "-1", "char", tmp_expression_value);
					}
				}
			}
			else {
				push_midcode(PRINTF_OP, "-1", "0", *s);
			}
		}
		else {
			return_type item_return_type = NOTHING;
			string tmp_expression_value = "";
			expression(&item_return_type, &tmp_expression_value);
			if (s) {
				if (item_return_type == INT) {
					push_midcode(PRINTF_OP, *s, "0", *s);
					push_midcode(PRINTF_OP, "-1", "int", tmp_expression_value);
				}
				else if (item_return_type == CHAR) {
					push_midcode(PRINTF_OP, *s, "0", *s);
					push_midcode(PRINTF_OP, "-1", "char", tmp_expression_value);
				}
			}
			else {
				if (item_return_type == INT) {
					push_midcode(PRINTF_OP, "-1", "int", tmp_expression_value);
				}
				else if (item_return_type == CHAR) {
					push_midcode(PRINTF_OP, "-1", "char", tmp_expression_value);
				}
			}
		}
	}
	if (sym == "RPARENT") {
		//out << "RPARENT )\n";
		getsym();
	}
	else {
		error_collect(RPARENT_ERROR, last_word_line);
	}
	//out << "<д���>\n";
}

// ���ַ����� ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���" //�ַ�����Ҫ��������һ���ַ�
void compiler::string_() {
	//out << "<�ַ���>\n";
	getsym();
}

// ������䣾 ::=  scanf '('����ʶ����')' 
void compiler::scanf_state(string name, string func_end_label) {
	//out << "SCANFTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		if (sym == "IDENFR") {
			//out << "IDENFR " << word << "\n";
			string name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (!whether_undefine_name(name)) {
				error_collect(UNDEFIEN_NAME_ERROR, line);
			}
			bool whether_find_in_local = false;
			for (int i = begin_func; i < symbol_num; i++) {
				if ((*(symbol_list[i])).name == name) {
					if ((*(symbol_list[i])).item_class_type == CONST) {
						error_collect(CONST_VALUE_CHANGE_ERROR, line);
						whether_find_in_local = true;
						break;
					}
 					if ((*(symbol_list[i])).item_return_type == INT) {
						push_midcode(SCANF_OP, name, "int", name);
						whether_find_in_local = true;
						//(*(symbol_list[i])).first_use = true;				// ���ǵ�
					}
					if ((*(symbol_list[i])).item_return_type == CHAR) {
						push_midcode(SCANF_OP, name, "char", name);
						whether_find_in_local = true;
						//(*(symbol_list[i])).first_use = true;				// ���ǵ�
					}
				}
			}
			if (!whether_find_in_local) {
				bool global = false;
				symbol* temp_sym = 0;
				def_global_value(name, &temp_sym, &global);
				if (global == true && temp_sym != 0) {
					if (temp_sym->item_class_type == CONST) {
						error_collect(CONST_VALUE_CHANGE_ERROR, line);
					}
					if (temp_sym->item_return_type == INT) {
						push_midcode(SCANF_OP, name, "int", name);
						//(*(symbol_list[i])).first_use = true;				// ���ǵ�
					}
					if (temp_sym->item_return_type == CHAR) {
						push_midcode(SCANF_OP, name, "char", name);
						//(*(symbol_list[i])).first_use = true;				// ���ǵ�
					}
				}
			}
			getsym();
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
			}
		}
	}
	//out << "<�����>\n";
}

// ��������䣾 ::= if '('��������')'����䣾��else����䣾��
void compiler::if_state(string name, string func_end_label) {
	//out << "IFTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();

		string label1 = "";
		Deal_with_label(&label1);

		string label2 = "";
		Deal_with_label(&label2);

		condition(label2, false);

		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		if_state1:
			statement(name, func_end_label);

			push_midcode(GOTO_OP, "", "", label1);
			push_midcode(LAB_OP, "", "", label2);
			
			if (sym == "ELSETK") {
				//out << "ELSETK " << word << "\n";
				getsym();
				statement(name, func_end_label);
			}
			push_midcode(LAB_OP, "", "", label1);
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
			goto if_state1;
		}
	}
	//out << "<�������>\n";
}

// ��ѭ����䣾 ::=  while '('��������')'����䣾| for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾    
void compiler::loop_state(string name, string func_end_label) {
	if (sym == "WHILETK") {
		//out << "WHILETK " << word << "\n";
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			string label1 = "";
			Deal_with_label(&label1);
			push_midcode(LAB_OP, "", "", label1);

			string label2 = "";
			Deal_with_label(&label2);
			condition(label2, false);


			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			loop_state1:
				statement(name, func_end_label);
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
				goto loop_state1;
			}
			push_midcode(GOTO_OP, "", "", label1);
			push_midcode(LAB_OP, "", "", label2);
		}
	}
	else if (sym == "FORTK") {
		//out << "FORTK " << word << "\n";
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				string name = word;
				transform(name.begin(), name.end(), name.begin(), ::tolower);
				if (!whether_undefine_name(name)) {
					error_collect(UNDEFIEN_NAME_ERROR, line);
				}
				symbol* sym1 = find_sym_Global(name);
				getsym();
				if (sym == "ASSIGN") {
					//out << "ASSIGN =\n";
					getsym();
					return_type item_return_type = NOTHING;

					string tmp_expression_value = "";
					expression(&item_return_type, &tmp_expression_value);

					if (whether_op_is_temp(tmp_expression_value)) {				////////////////////////////////û��
						mid_code_struct* code = mid_codes[mid_code_num - 1];
						code->result_name = sym1->name;
					}
					else {
						push_midcode(ADD_OP, tmp_expression_value, "0", sym1->name);
						/*
						for (int i = 0; i < symbol_num; i++) {
							if ((*(symbol_list[i])).name == name) {
								(*(symbol_list[i])).first_use = true;				/////////////////////// assign ����, ������֮ǰ��ֵ��
							}
						}
						*/
					}

					if (sym == "SEMICN") {
						//out << "SEMICN ;\n";
						getsym();
					FORK1:
						string label1 = "";
						Deal_with_label(&label1);

						string label2 = "";
						Deal_with_label(&label2);

						push_midcode(LAB_OP, "", "", label1);
						condition(label2, false);

						if (sym == "SEMICN") {
							//out << "SEMICN ;\n";
							getsym();
						FORK2:
							if (sym == "IDENFR") {
								//out << "IDENFR " << word << "\n";
								string name = word;
								transform(name.begin(), name.end(), name.begin(), ::tolower);
								if (!whether_undefine_name(name)) {
									error_collect(UNDEFIEN_NAME_ERROR, line);
								}
								symbol* sym2 = find_sym_Global(name);

								getsym();
								if (sym == "ASSIGN") {
									//out << "ASSIGN =\n";
									getsym();
									if (sym == "IDENFR") {
										//out << "IDENFR " << word << "\n";
										string name = word;
										transform(name.begin(), name.end(), name.begin(), ::tolower);
										if (!whether_undefine_name(name)) {
											error_collect(UNDEFIEN_NAME_ERROR, line);
										}
										symbol* sym3 = find_sym_Global(name);

										getsym();

										bool whether_PLUS_or_MINU = false;
										if (sym == "PLUS") {
											//out << "PLUS +\n";
											whether_PLUS_or_MINU = true;
											getsym();
										}
										if (sym == "MINU") {
											//out << "MINU -\n";
											whether_PLUS_or_MINU = false;
											getsym();
										}
										
										string foodstep_num = "";
										if (sym == "INTCON") {
											//out << "INTCON " << word << "\n";
											foodstep_num = word;
											getsym();
										}
										//out << "<�޷�������>\n";
										//out << "<����>\n";

										/*
										for (int i = 0; i < symbol_num; i++) {
											if ((*(symbol_list[i])).name == name) {
												(*(symbol_list[i])).first_use = true;				/////////////////////// assign ����, ������֮ǰ��ֵ��
											}
										}
										*/

										if (sym == "RPARENT") {
											//out << "RPARENT )\n";
											getsym();
										}
										else {
											error_collect(RPARENT_ERROR, last_word_line);
										}
										statement(name, func_end_label);

										push_midcode(whether_PLUS_or_MINU == true ? ADD_OP : SUB_OP, sym3->name, foodstep_num, sym2->name);
										push_midcode(GOTO_OP, "", "", label1);
										push_midcode(LAB_OP, "", "", label2);
									}
								}
							}
						}
						else {
							error_collect(SEMICN_ERROR, last_word_line);
							goto FORK2;
						}
					}
					else {
						error_collect(SEMICN_ERROR, last_word_line);
						goto FORK1;
					}
				}
			}
		}
	}
	//out << "<ѭ�����>\n";
}

void compiler::non_sign_integer() {
	if (sym == "INTCON") {
		//out << "INTCON " << word << "\n";
		getsym();
	}
	//out << "<�޷�������>\n";
}


// �������� ::= ���޷��������� 
void compiler::foodstep() {
	non_sign_integer();
	//out << "<����>\n";
}

// �������� ::=  �����ʽ������ϵ������������ʽ��
void compiler::condition(string label, bool flag) {
	return_type item_return_type = NOTHING;

	string operation1 = "";
	expression(&item_return_type, &operation1);
	if (item_return_type != INT) {
		error_collect(CONDITION_ERROR, line);
	}
	string sym_remember = sym;

	if (sym == "LSS") {
		//out << "LSS <\n";
		getsym();
	}
	else if (sym == "LEQ") {
		//out << "LEQ <=\n";
		getsym();
	}
	else if (sym == "GRE") {
		//out << "GRE >\n";
		getsym();
	}
	else if (sym == "GEQ") {
		//out << "GEQ >=\n";
		getsym();
	}
	else if (sym == "NEQ") {
		//out << "NEQ !=\n";
		getsym();
	}
	else if (sym == "EQL") {
		//out << "EQL ==\n";
		getsym();
	}
	item_return_type = NOTHING;

	string operation2 = "";
	expression(&item_return_type, &operation2);
	if (item_return_type != INT) {
		error_collect(CONDITION_ERROR, line);
	}

	
	if (sym_remember == "LSS") {
		push_midcode((flag == true) ? LSS_OP : GEQ_OP, operation1, operation2, label);
	}
	else if (sym_remember == "LEQ") {
		push_midcode((flag == true) ? LEQ_OP : GRE_OP, operation1, operation2, label);
	}
	else if (sym_remember == "GRE") {
		push_midcode((flag == true) ? GRE_OP : LEQ_OP, operation1, operation2, label);
	}
	else if (sym_remember == "GEQ") {
		push_midcode((flag == true) ? GEQ_OP : LSS_OP, operation1, operation2, label);
	}
	else if (sym_remember == "NEQ") {
		push_midcode((flag == true) ? NEQ_OP : EQL_OP, operation1, operation2, label);
	}
	else if (sym_remember == "EQL") {
		push_midcode((flag == true) ? EQL_OP : NEQ_OP, operation1, operation2, label);
	}
	
	

	//out << "<����>\n";
}

// �����ʽ�� ::= �ۣ������ݣ��{���ӷ�����������}
void compiler::expression(return_type* item_return_type, string* expression_value) {

	bool neg = false;
	string expression_value1 = "";

	if (sym == "PLUS") {
		//out << "PLUS +\n";
		getsym();
	}
	else if (sym == "MINU") {
		//out << "MINU -\n";
		neg = true;
		getsym();
	}
	term(item_return_type, &expression_value1);
	if (neg == true) {
		if (whether_op_is_num(expression_value1)) {
			expression_value1 = std::to_string((-1) * (stoi(expression_value1)));
		}
		else if (whether_op_is_char(expression_value1)) {
			std::string temp = std::string();
			Deal_with_temp(&temp);
			push_midcode(SUB_OP, "0", expression_value1, temp);
			expression_value1 = temp;
		}
		else {	//? what this case refers to?
			push_midcode(SUB_OP, "0", expression_value1, expression_value1);
		}
	}

	while (sym == "PLUS" || sym == "MINU") {
		bool is_add = false;
		if (sym == "PLUS") {
			//out << "PLUS +\n";
			is_add = true;
			getsym();
		}
		else if (sym == "MINU") {
			//out << "MINU -\n";
			getsym();
		}
		string expression_value2 = "";
		term(item_return_type, &expression_value2);
		*item_return_type = INT;	// only one �ַ����������з���ֵ�����������

		if (whether_op_is_num(expression_value1)) {
			if (whether_op_is_num(expression_value2)) {
				int result = 0;
				if (is_add) {
					result = stoi(expression_value1) + stoi(expression_value2);
				}
				else {
					result = stoi(expression_value1) - stoi(expression_value2);
				}
				expression_value1 = to_string(result);
			}
			else if (whether_op_is_char(expression_value2)) {
				std::string temp = std::string();
				Deal_with_temp(&temp);
				if (is_add) {
					push_midcode(ADD_OP, expression_value1, expression_value2, temp);
				}
				else {
					push_midcode(SUB_OP, expression_value1, expression_value2, temp);
				}
				expression_value1 = temp;
			}
			else {
				if (is_add) {
					push_midcode(ADD_OP, expression_value1, expression_value2, expression_value2); // expression_value2 �Ǳ��ʽ
				}
				else {
					push_midcode(SUB_OP, expression_value1, expression_value2, expression_value2);
				}
				expression_value1 = expression_value2;
			}
		}
		else if (whether_op_is_temp(expression_value1)) {	//????????????
			if (is_add) {
				push_midcode(ADD_OP, expression_value1, expression_value2, expression_value1);
			}
			else {
				push_midcode(SUB_OP, expression_value1, expression_value2, expression_value1);
			}
		}
		else {
			std::string temp = std::string();
			Deal_with_temp(&temp);
			if (is_add) {
				push_midcode(ADD_OP, expression_value1, expression_value2, temp);
			}
			else {
				push_midcode(SUB_OP, expression_value1, expression_value2, temp);
			}
			expression_value1 = temp;
		}
	}
	*expression_value = expression_value1;
	//out << "<���ʽ>\n";
}

// ��� ::= �����ӣ�{���˷�������������ӣ�} 
void compiler::term(return_type* item_return_type, string* expression_value) {

	string expression_value1 = "";
	factor(item_return_type, &expression_value1);

	while (sym == "MULT" || sym == "DIV") {

		bool is_mul = false;
		if (sym == "MULT") {
			is_mul = true;
			//out << "MULT *\n";
			getsym();
		}
		else if (sym == "DIV") {
			//out << "DIV /\n";
			getsym();
		}
		string expression_value2 = "";
		factor(item_return_type, &expression_value2);
		*item_return_type = INT;	// only one �ַ����������з���ֵ�����������

		if (whether_op_is_num(expression_value1)) {
			if (whether_op_is_num(expression_value2)) {
				int result = 0;
				if (is_mul) {
					result = stoi(expression_value1) * stoi(expression_value2);
				}
				else {
					result = stoi(expression_value1) / stoi(expression_value2);
				}
				expression_value1 = to_string(result);
			}
			else if (!whether_op_is_temp(expression_value1)) {	//?????????????????
				std::string temp = std::string();
				Deal_with_temp(&temp);
				if (is_mul) {
					push_midcode(MUL_OP, expression_value1, expression_value2, temp);
				}
				else {
					push_midcode(DIV_OP, expression_value1, expression_value2, temp);
				}
				expression_value1 = temp;
			}
			else {
				if (is_mul) {
					push_midcode(MUL_OP, expression_value1, expression_value2, expression_value2);
				}
				else {
					push_midcode(DIV_OP, expression_value1, expression_value2, expression_value2);
				}
				expression_value1 = expression_value2;
			}
		}
		else {
			if (whether_op_is_temp(expression_value1)) {
				if (is_mul) {
					push_midcode(MUL_OP, expression_value1, expression_value2, expression_value1);
				}
				else {
					push_midcode(DIV_OP, expression_value1, expression_value2, expression_value1);
				}
			}
			else {
				std::string temp = std::string();
				Deal_with_temp(&temp);
				if (is_mul) {
					push_midcode(MUL_OP, expression_value1, expression_value2, temp);
				}
				else {
					push_midcode(DIV_OP, expression_value1, expression_value2, temp);
				}
				expression_value1 = temp;
			}
		}
	}
	*expression_value = expression_value1;
	//out << "<��>\n";
}

// �����ӣ� ::= ����ʶ����������ʶ����'['�����ʽ��']'|����ʶ����'['�����ʽ��']''['�����ʽ��']'|'('�����ʽ��')'����������|���ַ��������з���ֵ����������䣾         
void compiler::factor(return_type* item_return_type, string* expression_value) {
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		string name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		symbol* sym_record = find_sym_Global(name);
		if (!whether_undefine_name(name)) {
			error_collect(UNDEFIEN_NAME_ERROR, line);
			*item_return_type = NOTHING;
		}
		else {
			if (get_return_type(name) == CHAR) {
				*item_return_type = CHAR;	///////////////////////////////////////////////////////////////
			}
			else {
				*item_return_type = INT;
			}
		}
		getsym();
		if (sym == "LBRACK") {
			//out << "LBRACK [\n";
			getsym();
			return_type item_return_type = NOTHING;
			string expression_value1 = "";	//
			expression(&item_return_type, &expression_value1);
			if (item_return_type != INT) {
				error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
			}
			if (sym == "RBRACK") {
				//out << "RBRACK ]\n";
				getsym();
			factor1:
				if (sym == "LBRACK") {
					//out << "LBRACK [\n";
					getsym();
					return_type item_return_type = NOTHING;
					string expression_value2 = "";
					expression(&item_return_type, &expression_value2);
					if (item_return_type != INT) {
						error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
					}
					if (sym == "RBRACK") {
						//out << "RBRACK ]\n";
						getsym();
					}
					else {
						error_collect(RBRACK_ERROR, last_word_line);
					}

					Deal_with_temp(expression_value); 
					
					int lie = 0;
					for (int i = 0; i < symbol_num; i++) {			//////////////��ʼλ�ÿ��ܴ���
						if ((*(symbol_list[i])).name == name) {
							lie = (*(symbol_list[i])).array_length_lie;
						}
					}
					push_midcode_special(RIGHT_DOUBLE_ARRAY_OP, name, expression_value1, expression_value2, *expression_value, lie);
				}
				else {
					Deal_with_temp(expression_value);
					push_midcode(RIGHT_ARRAY_OP, name, expression_value1, *expression_value);
				}
			}
			else {
				error_collect(RBRACK_ERROR, last_word_line);
				goto factor1;
			}
		}
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "SEMICN" || sym == "COMMA") {	// ���� c(;;;;; ��ɵ�ƴ�
				error_collect(RPARENT_ERROR, last_word_line);
			}
			else {
				value_para(name);
				push_midcode(USE_FUNC_OP, "", "", name);
				now_func->has_func = now_func->has_func + 1;
				
				*expression_value = "#RET";
				std::string temp = std::string();
				Deal_with_temp(&temp);
				///////////////////////////////////////////////if (sym->ret_type == CHARRET) 
				push_midcode(ADD_OP, *expression_value, "0", temp);
				*expression_value = temp;
				if (sym == "RPARENT") {
					//out << "RPARENT )\n";
					getsym();
				}
				else {
					error_collect(RPARENT_ERROR, last_word_line);
				}
			}
			//out << "<�з���ֵ�����������>\n";
		}
		else {
			if (sym_record->item_class_type == VAR || sym_record->item_class_type == PARA) {

				/*
				if (sym_record->first_use == false) {
					push_midcode(ADD_OP, to_string(sym_record->value), "0", name);
				}
				*/

				symbol* tmp_sym = find_sym_Local(name);
				if (tmp_sym == 0) {
					std::string temp = std::string();
					Deal_with_temp(&temp);
					push_midcode(ADD_OP, name, "0", temp);
					*expression_value = temp;
					now_func->has_global = true;
				}
				else {
					*expression_value =name;
				}
			}
			else if (sym_record->item_class_type == CONST) {
				*expression_value = to_string(sym_record->value);
			}
		}
	}
	else if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		*item_return_type = INT;
		getsym();
		return_type item_return_type = NOTHING;
		expression(&item_return_type, expression_value);
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}
	}
	else if (sym == "CHARCON") {
		//out << "CHARCON " << word << "\n";
		*expression_value = to_string(word[0]);
		*item_return_type = CHAR;
		getsym();
	}
	else {
		*item_return_type = INT;
		int value = 0;
		integer(&value);
		*expression_value = to_string(value);
	}
	//out << "<����>\n";
}

// ���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')' 
void compiler::have_return_use(string name) {
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		if (sym == "SEMICN" || sym == "COMMA") {	// ���� c(;;;;; ��ɵ�ƴ�
			error_collect(RPARENT_ERROR, last_word_line);
		}
		else {
			value_para(name);
			push_midcode(USE_FUNC_OP, "", "", name);
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
			}
		}
	}
	//out << "<�з���ֵ�����������>\n";
}

/*
void compiler::non_return_use() {
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		value_para();
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR);
		}
	}
	//out << "<�޷���ֵ�����������>\n";
}
*/

// ��ֵ������ ::= �����ʽ��{,�����ʽ��}�����գ�   
void compiler::value_para(string name) {
	whether_use_func_para_num_right = true;
	whether_use_func_para_type_right = true;

	int now_func_symbol_num = 0;
	int now_func_para_num_expected = 0;
	for (int i = 0; i < symbol_num; i++) {
		if ((*symbol_list[i]).name == name) {
			now_func_symbol_num = i;
			now_func_para_num_expected = (*symbol_list[i]).para_num;
			break;
		}
	}

	int now_func_para_num = 0;

	if (sym != "RPARENT") {
		return_type item_return_type = NOTHING;

		string tmp_expression_value = "";
		expression(&item_return_type, &tmp_expression_value);
		push_midcode(REAL_PARA_OP, tmp_expression_value, "0", tmp_expression_value);

		if ((*symbol_list[now_func_symbol_num]).para_list[now_func_para_num] != item_return_type) {
			whether_use_func_para_type_right = false;
		}
		now_func_para_num++;
		
		while (sym == "COMMA") {
			//out << "COMMA ,\n";
			getsym();
			return_type item_return_type = NOTHING;
			string tmp_expression_value = "";
			expression(&item_return_type, &tmp_expression_value);
			push_midcode(REAL_PARA_OP, tmp_expression_value, "0", tmp_expression_value);

			if ((*symbol_list[now_func_symbol_num]).para_list[now_func_para_num] != item_return_type) {
				whether_use_func_para_type_right = false;
			}
			now_func_para_num++;
		}
		if (now_func_para_num_expected != now_func_para_num) {
			whether_use_func_para_num_right = false;
		}

		if (!whether_use_func_para_num_right) {
			error_collect(FUNC_PARA_NUM_ERROR, line);
		}
		if (whether_use_func_para_num_right && (!whether_use_func_para_type_right)) {	
			error_collect(FUNC_PARA_TYPE_ERROR, line);
		}
	}
	else {		/////////////////// shit();
		if (now_func_para_num_expected != 0) {
			error_collect(FUNC_PARA_NUM_ERROR, line);
		}
	}
	//out << "<ֵ������>\n";
}

void compiler::analysis_first_letter() {
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		sym2 = sym;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		sym2 = sym;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		getsym();
	}
}

/*
#include "compiler.h"

void compiler::memorize_two_steps() {

	now_point = in.tellg();
	word1 = word;
	sym1 = sym;
	getsym();
	word2 = word;
	sym2 = sym;
	getsym();
}

void compiler::goesback_two_steps() {
	sym = sym1;
	word = word1;
	in.seekg(now_point, ios::beg);
}

void compiler::program() {
	getsym();
	if (sym == "CONSTTK") {
		//out << "CONSTTK " << word << "\n";
		const_discription();
	}
	temp_line = line;
	memorize_two_steps();

	while (sym != "LPARENT") {
		goesback_two_steps();
		line = temp_line;
		var_discription();

		temp_line = line;
		memorize_two_steps();
	}

	while (sym2 != "MAINTK") {
		goesback_two_steps();
		line = temp_line;

		if (sym == "VOIDTK") {
			enter_func();
			non_return();
		}
		else {
			class_type item_class_type = FUNC;
			return_type item_return_type = NOTHING;
			string name;
			if (sym == "INTTK") {
				//out << "INTTK " << word << "\n";
				item_return_type = INT;
				sym2 = sym;
				getsym();
			}
			else if (sym == "CHARTK") {
				//out << "CHARTK " << word << "\n";
				item_return_type = CHAR;
				sym2 = sym;
				getsym();
			}
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				name = word;
				transform(name.begin(), name.end(), name.begin(), ::tolower);
				getsym();
			}
			if (sym == "LPARENT") {
				//out << "<����ͷ��>\n";
				//out << "LPARENT (\n";
				have_return_func.push_back(word);
				getsym();
				enter_func();
				have_return(name, item_class_type, item_return_type);
			}
		}
		temp_line = line;
		memorize_two_steps();
	}
	goesback_two_steps();
	line = temp_line;

	//out << "VOIDTK " << word << "\n";
	getsym();

	class_type item_class_type = FUNC;
	return_type item_return_type = VOID;
	string name = "main";
	if (whether_redefine_name_when_use_FUNC(name)) {
		error_collect(REDEFINE_NAME_ERROR, line);
	}
	else {
		push_sym_list(name, item_class_type, item_return_type, 0, line);
	}
	enter_func();
	main_func();
	pop_sym_list();
	//out << "<������>\n";
	//out << "<����>\n";
}


void compiler::var_discription() {

	while (sym == "INTTK" || sym == "CHARTK") {

		int temp_line = line;
		memorize_two_steps();

		if (sym != "LPARENT") {
			goesback_two_steps();
			line = temp_line;
			var_defination();
		}
		else {
			line = temp_line;
			goesback_two_steps();
			break;
		}
	}
	//out << "<����˵��>\n";
}

void compiler::var_defination() {
	temp_line = line;
	now_point = in.tellg();
	sym1 = sym;
	bool whether_have_equal = false;
	while (sym != "SEMICN" && last_word_line == line) {		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		if (sym == "ASSIGN") {
			whether_have_equal = true;
			goesback_two_steps();	//ʵ����������goesback_one_step
			line = temp_line;
			var_def_init();
			//break;
		}
		else {
			getsym();
		}
	}
	if (!whether_have_equal) {
		goesback_two_steps();	//ʵ����������goesback_one_step
		line = temp_line;
		var_def_non_init();
	}
	//out << "<��������>\n";
	if (sym == "SEMICN") {
		//out << "SEMICN ;\n";
		getsym();
	}
	else {
		error_collect(SEMICN_ERROR, last_word_line);
	}
}

void compiler::var_def_init() {
	class_type item_class_type = VAR;
	return_type item_return_type = NOTHING;
	string name;
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		sym2 = sym;
		item_return_type = INT;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		sym2 = sym;
		item_return_type = CHAR;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		getsym();
	}
	if (sym == "ASSIGN") {
		//out << "ASSIGN =\n";
		getsym();
		if ( item_return_type != constant()) {
			error_collect(CONST_VALUE_TYPE_ERROR, line);
		}
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line);
		}
	}
	if (sym == "LBRACK") {			////////////////////////////////////////////////////else if
		//out << "LBRACK [\n";
		item_class_type = ARRAY;
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line);
		}

		getsym();

		int hang_expected = 0;
		if (sym == "INTCON") {
			//out << "INTCON " << word << "\n";
			hang_expected = std::stoi(word);
			getsym();
		}
		else {
			error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
			getsym();
		}
		if (sym == "RBRACK") {
			//out << "RBRACK ]\n";
			getsym();
		var_def_init1:
			if (sym == "ASSIGN") {
				//out << "ASSIGN =\n";
				getsym();
				if (sym == "LBRACE") {
					//out << "LBRACE {\n";
					getsym();
					int volume1 = 0;
					if (item_return_type != constant()) {
						error_collect(CONST_VALUE_TYPE_ERROR, line);
					}
					volume1 = volume1 + 1;
					while (sym == "COMMA") {
						//out << "COMMA ,\n";
						getsym();
						if (item_return_type != constant()) {
							error_collect(CONST_VALUE_TYPE_ERROR, line);
						}
						volume1 = volume1 + 1;
					}
					if (sym == "RBRACE") {
						//out << "RBRACE }\n";
						getsym();
					}
					if (hang_expected != volume1) {
						error_collect(ARRAY_INIT_NUM_ERROR, line);
					}
				}
				else {
					error_collect(ARRAY_INIT_NUM_ERROR, line);
					getsym(); //////////////////////////////////////////////////////////////////////��ά����Ψһ����
				}
			}


			if (sym == "LBRACK") {
				//out << "LBRACK [\n";
				int lie_expected = 0;
				getsym();
				if (sym == "INTCON") {
					//out << "INTCON " << word << "\n";
					lie_expected = std::stoi(word);
					getsym();
				}
				else {
					error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
					getsym();
				}
				if (sym == "RBRACK") {
					//out << "RBRACK ]\n";
					getsym();
				var_def_init2:
					if (sym == "ASSIGN") {
						//out << "ASSIGN =\n";
						getsym();
						if (sym == "LBRACE") {
							//out << "LBRACE {\n";
							getsym();

							if (sym == "LBRACE") {
								//out << "LBRACE {\n";
								int hang_volume = 0;
								hang_volume = hang_volume + 1;
								getsym();
								int lie_volume = 0;
								if (item_return_type != constant()) {
									error_collect(CONST_VALUE_TYPE_ERROR, line);
								}
								lie_volume = lie_volume + 1;
								while (sym == "COMMA") {
									//out << "COMMA ,\n";
									getsym();
									if (item_return_type != constant()) {
										error_collect(CONST_VALUE_TYPE_ERROR, line);
									}
									lie_volume = lie_volume + 1;
								}
								if (sym == "RBRACE") {
									//out << "RBRACE }\n";
									if (lie_expected != lie_volume) {
										error_collect(ARRAY_INIT_NUM_ERROR, line);
									}
									lie_volume = 0;
									getsym();

									while (sym == "COMMA") {
										//out << "COMMA ,\n";
										hang_volume = hang_volume + 1;
										getsym();
										if (sym == "LBRACE") {
											//out << "LBRACE {\n";
											lie_volume = 0;
											getsym();
											if (item_return_type != constant()) {
												error_collect(CONST_VALUE_TYPE_ERROR, line);
											}
											lie_volume = lie_volume + 1;
											while (sym == "COMMA") {
												//out << "COMMA ,\n";
												getsym();
												if (item_return_type != constant()) {
													error_collect(CONST_VALUE_TYPE_ERROR, line);
												}
												lie_volume = lie_volume + 1;
												if (sym == "RBRACE") {
													//out << "RBRACE }\n";
													if (lie_expected != lie_volume) {
														error_collect(ARRAY_INIT_NUM_ERROR, line);
													}
													lie_volume = 0;
													getsym();
													break;
												}
											}
										}
									}
									if (hang_expected != hang_volume){
										error_collect(ARRAY_INIT_NUM_ERROR, line);			////////////////////////////////////���治��getsym()
									}
									if (sym == "RBRACE") {
										//out << "RBRACE }\n";
										getsym();
									}
								}
							}
							else {
								error_collect(ARRAY_INIT_NUM_ERROR, line);
								for (int i = 0; i <= lie_expected; i++) {
									getsym();							/////////////////////////���� ��2}�� , �Ƶ���һ��getsym
								}
							}
						}
						else {
							error_collect(ARRAY_INIT_NUM_ERROR, line);
							getsym();								////////////////////////������һ������
						}
					}
				}
				else {
					error_collect(RBRACK_ERROR, last_word_line);
					goto var_def_init2;
				}
			}
		}
		else {
			error_collect(RBRACK_ERROR, last_word_line);
			goto var_def_init1;
		}
	}
	//out << "<�������弰��ʼ��>\n";
}

void compiler::var_def_non_init() {
	class_type item_class_type = VAR;
	return_type item_return_type = NOTHING;
	string name;
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		sym2 = sym;
		item_return_type = INT;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		sym2 = sym;
		item_return_type = CHAR;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		getsym();
	}

	if (sym == "LBRACK") {
		//out << "LBRACK [\n";
		item_class_type = ARRAY;
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line);
		}

		getsym();

		//�±�
		if (sym == "INTCON") {
			//out << "INTCON " << word << "\n";
			getsym();
		}
		else {
			error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
			getsym();
		}

		if (sym == "RBRACK") {
			//out << "RBRACK ]\n";
			getsym();
		var_def_non_init1:
			if (sym == "LBRACK") {
				//out << "LBRACK [\n";
				getsym();

				//�±�
				if (sym == "INTCON") {
					//out << "INTCON " << word << "\n";
					getsym();
				}
				else {
					error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
					getsym();
				}

				if (sym == "RBRACK") {
					//out << "RBRACK ]\n";
					getsym();
				}
				else {
					error_collect(RBRACK_ERROR, last_word_line);
				}
			}
		}
		else {
			error_collect(RBRACK_ERROR, last_word_line);
			goto var_def_non_init1;
		}
	}
	else {
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line);
		}
	}

	while (sym == "COMMA") {

		//out << "COMMA ,\n";
		getsym();

		if (sym == "IDENFR") {
			//out << "IDENFR " << word << "\n";
			name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			getsym();
		}

		if (sym == "LBRACK") {
			//out << "LBRACK [\n";
			item_class_type = ARRAY;
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line);
			}
			getsym();

			//�±�
			if (sym == "INTCON") {
				//out << "INTCON " << word << "\n";
				getsym();
			}
			else {
				error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
				getsym();
			}

			if (sym == "RBRACK") {
				//out << "RBRACK ]\n";
				getsym();
			var_def_non_init2:
				if (sym == "LBRACK") {
					//out << "LBRACK [\n";
					getsym();

					//�±�
					if (sym == "INTCON") {
						//out << "INTCON " << word << "\n";
						getsym();
					}
					else {
						error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
						getsym();
					}

					if (sym == "RBRACK") {
						//out << "RBRACK ]\n";
						getsym();
					}
					else {
						error_collect(RBRACK_ERROR, last_word_line);
					}
				}
			}
			else {
				error_collect(RBRACK_ERROR, last_word_line);
				goto var_def_non_init2;
			}
		}
		else {
			item_class_type = VAR;
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line);
			}
		}
	}
	//out << "<���������޳�ʼ��>\n";
}

void compiler::main_func() {
	if (sym == "MAINTK") {
		//out << "MAINTK " << word << "\n";
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			main_func1:
				if (sym == "LBRACE") {
					//out << "LBRACE {\n";
					getsym();

					now_func_return_type_expected = VOID;
					counpund();

					if (sym == "RBRACE") {
						//out << "RBRACE }\n";
						getsym();
					}
				}
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
				goto main_func1;
			}
		}
	}
}

void compiler::const_discription() {
	do {
		getsym();
		const_defination();
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		const_discription1:
			if (sym == "CONSTTK") {
				//out << "CONSTTK " << word << "\n";
			}
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
			goto const_discription1;
		}
	} while (sym == "CONSTTK");
	//out << "<����˵��>\n";
}

void compiler::const_defination() {
	class_type item_class_type = CONST;
	if (sym == "INTTK") {
		return_type item_return_type = INT;
		//out << "INTTK " << word << "\n";
		do {
			getsym();
			string name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line);
			}
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				getsym();
				if (sym == "ASSIGN") {
					//out << "ASSIGN =\n";
					getsym();
				}
				if (sym == "PLUS") {
					//out << "PLUS +\n";
					getsym();
				}
				else if (sym == "MINU") {
					//out << "MINU -\n";
					getsym();
				}
				if (sym == "INTCON") {
					//out << "INTCON " << word << "\n";
					getsym();
					//out << "<�޷�������>\n";
				}
			}
			if (sym == "COMMA") {
				//out << "COMMA ,\n";
			}
		} while (sym == "COMMA");
	}
	else if (sym == "CHARTK") {
		return_type item_return_type = CHAR;
		//out << "CHARTK " << word << "\n";
		do {
			getsym();
			string name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (whether_redefine_name_when_use_IDENFR(name)) {
				error_collect(REDEFINE_NAME_ERROR, line);
			}
			else {
				push_sym_list(name, item_class_type, item_return_type, 0, line);
			}
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				getsym();
				if (sym == "ASSIGN") {
					//out << "ASSIGN =\n";
					getsym();
				}
				if (sym == "CHARCON") {
					//out << "CHARCON " << word << "\n";
					getsym();
				}
			}
			if (sym == "COMMA") {
				//out << "COMMA ,\n";
			}
		} while (sym == "COMMA");
	}
	//out << "<��������>\n";
}

void compiler::integer() {
	if (sym == "PLUS") {
		//out << "PLUS +\n";
		getsym();
	}
	else if (sym == "MINU") {
		//out << "MINU -\n";
		getsym();
	}
	non_sign_integer();
	//out << "<����>\n";
}

void compiler::non_sign_integer() {
	if (sym == "INTCON") {
		//out << "INTCON " << word << "\n";
		getsym();
	}
	//out << "<�޷�������>\n";
}

void compiler::non_return() {

	bool whether_add_func = false;

	//out << "VOIDTK " << word << "\n";
	class_type item_class_type = FUNC;
	return_type item_return_type = VOID;
	string name;
	getsym();
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
		}

		if (whether_redefine_name_when_use_FUNC(name)) {
			whether_add_func = false;
			error_collect(REDEFINE_NAME_ERROR, line);
			int para_num = 0;
			paratable(&para_num, -1);
		}
		else {
			whether_add_func = true;
			int this_func_symbol_num = symbol_num;
			push_sym_list(name, item_class_type, item_return_type, 0, line);
			int para_num = 0;
			paratable(&para_num, this_func_symbol_num);
			(*(symbol_list[this_func_symbol_num])).para_num = para_num;
		}

		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}
		if (sym == "LBRACE") {
			//out << "LBRACE {\n";
			getsym();
		}

		now_func_return_type_expected = VOID;
		counpund();

		if (sym == "RBRACE") {
			//out << "RBRACE }\n";
			getsym();
		}

		if (!whether_add_func) {
			symbol_num = last_func;
			last_func = 0;
		}
		else {
			pop_sym_list();
		}
	}
	//out << "<�޷���ֵ��������>\n";
}

void compiler::have_return(string name, class_type item_class_type, return_type item_return_type) {

	bool whether_add_func = false;

	if (whether_redefine_name_when_use_FUNC(name)) {
		whether_add_func = false;
		error_collect(REDEFINE_NAME_ERROR, line);
		int para_num = 0;
		paratable(&para_num, -1);
	}
	else {
		whether_add_func = true;
		int this_func_symbol_num = symbol_num;
		push_sym_list(name, item_class_type, item_return_type, 0, line);	///////////////////////////////////////////////
		int para_num = 0;
		paratable(&para_num, this_func_symbol_num);
		(*(symbol_list[this_func_symbol_num])).para_num = para_num;
	}

	now_func_return_type_expected = item_return_type;

	if (sym == "RPARENT") {
		//out << "RPARENT )\n";
		getsym();
	have_return1:
		if (sym == "LBRACE") {
			//out << "LBRACE {\n";
			getsym();

			now_func_return_type = NOTHING;
			counpund();
			if (now_func_return_type == NOTHING) {
				error_collect(HAVE_RETURN_FUNC_ERROR, line);
			}

			if (sym == "RBRACE") {
				//out << "RBRACE }\n";
				getsym();
			}
		}
	}
	else {
		error_collect(RPARENT_ERROR, last_word_line);
		goto have_return1;
	}

	if (!whether_add_func) {
		symbol_num = last_func;
		last_func = 0;
	}
	else {
		pop_sym_list();
	}
	//out << "<�з���ֵ��������>\n";
}

void compiler::paratable(int* para_num, int this_func_symbol_num) {
	para(para_num, this_func_symbol_num);
	while (sym == "COMMA") {
		//out << "COMMA ,\n";
		getsym();
		para(para_num, this_func_symbol_num);
	}
	//out << "<������>\n";
}

void compiler::para(int* para_num, int this_func_symbol_num) {
	class_type item_class_type = PARA;
	return_type item_return_type = NOTHING;
	string name;
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		item_return_type = INT;
		(*symbol_list[this_func_symbol_num]).para_list[*para_num] = INT;
		*para_num = *para_num + 1;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		item_return_type = CHAR;
		(*symbol_list[this_func_symbol_num]).para_list[*para_num] = CHAR;
		*para_num = *para_num + 1;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (whether_redefine_name_when_use_IDENFR(name)) {
			error_collect(REDEFINE_NAME_ERROR, line);
		}
		else {
			push_sym_list(name, item_class_type, item_return_type, 0, line);
		}
		getsym();
	}
}

void compiler::counpund() {
	if (sym == "CONSTTK") {
		//out << "CONSTTK " << word << "\n";
		const_discription();
	}
	if (sym == "INTTK" || sym == "CHARTK") {
		var_discription();
	}
	statements();
	//out << "<�������>\n";
}

void compiler::statements() {
	while (sym == "WHILETK" || sym == "FORTK" || sym == "IFTK" || sym == "IDENFR"
		|| sym == "SCANFTK" || sym == "PRINTFTK" || sym == "SWITCHTK"
		|| sym == "SEMICN" || sym == "RETURNTK" || sym == "LBRACE") {
		statement();
	}
	//out << "<�����>\n";
}

void compiler::statement() {					// switch
	if (sym == "WHILETK") {
		loop_state();
	}
	else if (sym == "FORTK") {
		loop_state();
	}
	else if (sym == "IFTK") {
		if_state();
	}
	else if (sym == "IDENFR") {
		bool whether_return = false;
		//out << "IDENFR " << word << "\n";
		string name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (!whether_undefine_name(name)) {
			error_collect(UNDEFIEN_NAME_ERROR, line);
		}
		if (find(have_return_func.begin(), have_return_func.end(), word) != have_return_func.end()) {
			whether_return = true;
		}
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "SEMICN" || sym == "COMMA") {	// ���� c(;;;;; ��ɵ�ƴ�
				error_collect(RPARENT_ERROR, last_word_line);
			}
			else {
				value_para(name);
				if (sym == "RPARENT") {
					//out << "RPARENT )\n";
					getsym();
				statement1:
					if (whether_return) {
						//have_return_use();		////////////////////////////////////////////////////////
						//out << "<�з���ֵ�����������>\n";
					}
					else {
						//non_return_use();		/////////////////////////////////////////////////////////
						//out << "<�޷���ֵ�����������>\n";
					}
				}
				else {
					error_collect(RPARENT_ERROR, last_word_line);
					goto statement1;
				}
				if (sym == "SEMICN") {
					//out << "SEMICN ;\n";
					getsym();
				}
				else {
					error_collect(SEMICN_ERROR, last_word_line);
				}
			}
		}
		else if (sym == "ASSIGN" || sym == "LBRACK") {
			for (int i = 0; i < symbol_num; i++) {
				if ((*(symbol_list[i])).name == name) {
					if ((*(symbol_list[i])).item_class_type == CONST) {
						error_collect(CONST_VALUE_CHANGE_ERROR, line);
						break;
					}
				}
			}
			assign_state();
		}

		else if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {											//////////////////////////
			error_collect(SEMICN_ERROR, last_word_line);
		}



	}
	else if (sym == "SCANFTK") {
		scanf_state();
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
		}
	}
	else if (sym == "PRINTFTK") {
		printf_state();
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
		}
	}
	else if (sym == "SWITCHTK") {
		switch_state();
	}
	else if (sym == "SEMICN") {
		//out << "SEMICN ;\n";
		getsym();

	}
	else if (sym == "RETURNTK") {
		return_state();
		if (sym == "SEMICN") {
			//out << "SEMICN ;\n";
			getsym();
		}
		else {
			error_collect(SEMICN_ERROR, last_word_line);
		}
	}
	else if (sym == "LBRACE") {
		//out << "LBRACE {\n";
		getsym();
		statements();
		if (sym == "RBRACE") {
			//out << "RBRACE }\n";
			getsym();
		}
	}
	else {
		error_collect(SEMICN_ERROR, last_word_line);	/////////////////////////////////
	}
	//out << "<���>\n";
}

void compiler::assign_state() {
	if (sym == "ASSIGN") {
		//out << "ASSIGN =\n";
		getsym();
		return_type item_return_type = NOTHING;
		expression(&item_return_type);
	}
	else if (sym == "LBRACK") {
		//out << "LBRACK [\n";
		getsym();
		return_type item_return_type = NOTHING;
		expression(&item_return_type);
		if (item_return_type != INT) {
			error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
		}
		if (sym == "RBRACK") {
			//out << "RBRACK ]\n";
			getsym();
		assign_state1:
			if (sym == "ASSIGN") {
				//out << "ASSIGN =\n";
				getsym();
				return_type item_return_type = NOTHING;
				expression(&item_return_type);
			}
			if (sym == "LBRACK") {
				//out << "LBRACK [\n";
				getsym();
				return_type item_return_type = NOTHING;
				expression(&item_return_type);
				if (item_return_type != INT) {
					error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
				}
				if (sym == "RBRACK") {
					//out << "RBRACK ]\n";
					getsym();
				assign_state2:
					if (sym == "ASSIGN") {
						//out << "ASSIGN =\n";
						getsym();
						return_type item_return_type = NOTHING;
						expression(&item_return_type);
					}
				}
				else {
					error_collect(RBRACK_ERROR, last_word_line);
					goto assign_state2;
				}
			}
		}
		else {
			error_collect(RBRACK_ERROR, last_word_line);
			goto assign_state1;
		}


	}
	//out << "<��ֵ���>\n";
	if (sym == "SEMICN") {
		//out << "SEMICN ;\n";
		getsym();
	}
	else {
		error_collect(SEMICN_ERROR, last_word_line);
	}

}


void compiler::return_state() {
	if (sym == "RETURNTK") {
		//out << "RETURNTK " << word << "\n";
		getsym();
	}
	if (sym == "LPARENT") {
		if (now_func_return_type_expected == VOID) {
			error_collect(NON_RETURN_FUNC_ERROR, line);
		}
		//out << "LPARENT (\n";
		getsym();
		if (sym != "RPARENT") {
			return_type item_return_type = NOTHING;
			expression(&item_return_type);
			now_func_return_type = item_return_type;
		}
		else {
			now_func_return_type = NOTHING;
		}
		if ((now_func_return_type_expected != now_func_return_type)&& (now_func_return_type_expected == INT || now_func_return_type_expected == CHAR)) {
			error_collect(HAVE_RETURN_FUNC_ERROR, line);
			now_func_return_type = HAVE_ERROR_COLLECT;
		}
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}
	}
	else {
		if (now_func_return_type_expected == INT || now_func_return_type_expected == CHAR) {
			error_collect(HAVE_RETURN_FUNC_ERROR, line);
			now_func_return_type = HAVE_ERROR_COLLECT;
		}
	}
	//out << "<�������>\n";
}

void compiler::switch_state() {
	//out << "SWITCHTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		return_type item_return_type = NOTHING;
		expression(&item_return_type);
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}

		if (sym == "LBRACE") {
			//out << "LBRACE {\n";
			getsym();
			case_table(item_return_type);
			default_table();
		}
		if (sym == "RBRACE") {
			//out << "RBRACE }\n";
			getsym();
		}
	}

	//out << "<������>\n";
}

void compiler::default_table() {
	if (sym == "DEFAULTTK") {
		//out << "DEFAULTTK " << word << "\n";
		getsym();
	}
	else {
		error_collect(DEFAULT_DISMISS_ERROR, line);
		goto default_table1;
	}
	if (sym == "COLON") {
		//out << "COLON :\n";
		getsym();
	}
	statement();
	//out << "<ȱʡ>\n";
default_table1:
	;
}

void compiler::case_table(return_type item_return_type) {
	while (sym == "CASETK") {
		case_substate(item_return_type);
	}
	//out << "<�����>\n";
}

void compiler::case_substate(return_type item_return_type) {
	if (sym == "CASETK") {
		//out << "CASETK " << word << "\n";
		getsym();
		if (item_return_type != constant()) {
			error_collect(CONST_VALUE_TYPE_ERROR, line);
		}
		if (sym == "COLON") {
			//out << "COLON :\n";
			getsym();
			statement();
		}
	}
	//out << "<��������>\n";
}

return_type compiler::constant() {
	if (sym == "CHARCON") {
		//out << "CHARCON " << word << "\n";
		getsym();
		return CHAR;
	}
	else {
		integer();
		return INT;
	}
	//out << "<����>\n";
}

void compiler::printf_state() {
	//out << "PRINTFTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		if (sym == "STRCON") {
			//out << "STRCON " << word << "\n";
			string_();
			if (sym == "COMMA") {
				//out << "COMMA ,\n";
				getsym();
				return_type item_return_type = NOTHING;
				expression(&item_return_type);
			}
		}
		else {
			return_type item_return_type = NOTHING;
			expression(&item_return_type);
		}
	}
	if (sym == "RPARENT") {
		//out << "RPARENT )\n";
		getsym();
	}
	else {
		error_collect(RPARENT_ERROR, last_word_line);
	}
	//out << "<д���>\n";
}

void compiler::string_() {
	//out << "<�ַ���>\n";
	getsym();
}

void compiler::scanf_state() {
	//out << "SCANFTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		if (sym == "IDENFR") {
			//out << "IDENFR " << word << "\n";
			string name = word;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (!whether_undefine_name(name)) {
				error_collect(UNDEFIEN_NAME_ERROR, line);
			}
			for (int i = symbol_num - 1; i >= 0; i--) {
				if ((*(symbol_list[i])).name == name) {
					if ((*(symbol_list[i])).item_class_type == CONST) {
						error_collect(CONST_VALUE_CHANGE_ERROR, line);
						break;
					}
				}
			}
			getsym();
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
			}
		}
	}
	//out << "<�����>\n";
}

void compiler::if_state() {
	//out << "IFTK " << word << "\n";
	getsym();
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		condition();
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		if_state1:
			statement();
			if (sym == "ELSETK") {
				//out << "ELSETK " << word << "\n";
				getsym();
				statement();
			}
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
			goto if_state1;
		}
	}
	//out << "<�������>\n";
}

void compiler::loop_state() {
	if (sym == "WHILETK") {
		//out << "WHILETK " << word << "\n";
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			condition();
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			loop_state1:
				statement();
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
				goto loop_state1;
			}
		}
	}
	else if (sym == "FORTK") {
		//out << "FORTK " << word << "\n";
		getsym();
		if (sym == "LPARENT") {
			//out << "LPARENT (\n";
			getsym();
			if (sym == "IDENFR") {
				//out << "IDENFR " << word << "\n";
				string name = word;
				transform(name.begin(), name.end(), name.begin(), ::tolower);
				if (!whether_undefine_name(name)) {
					error_collect(UNDEFIEN_NAME_ERROR, line);
				}
				getsym();
				if (sym == "ASSIGN") {
					//out << "ASSIGN =\n";
					getsym();
					return_type item_return_type = NOTHING;
					expression(&item_return_type);
					if (sym == "SEMICN") {
						//out << "SEMICN ;\n";
						getsym();
					FORK1:
						condition();
						if (sym == "SEMICN") {
							//out << "SEMICN ;\n";
							getsym();
						FORK2:
							if (sym == "IDENFR") {
								//out << "IDENFR " << word << "\n";
								string name = word;
								transform(name.begin(), name.end(), name.begin(), ::tolower);
								if (!whether_undefine_name(name)) {
									error_collect(UNDEFIEN_NAME_ERROR, line);
								}
								getsym();
								if (sym == "ASSIGN") {
									//out << "ASSIGN =\n";
									getsym();
									if (sym == "IDENFR") {
										//out << "IDENFR " << word << "\n";
										string name = word;
										transform(name.begin(), name.end(), name.begin(), ::tolower);
										if (!whether_undefine_name(name)) {
											error_collect(UNDEFIEN_NAME_ERROR, line);
										}
										getsym();
										if (sym == "PLUS") {
											//out << "PLUS +\n";
											getsym();
										}
										if (sym == "MINU") {
											//out << "MINU -\n";
											getsym();
										}
										foodstep();
										if (sym == "RPARENT") {
											//out << "RPARENT )\n";
											getsym();
										}
										else {
											error_collect(RPARENT_ERROR, last_word_line);
										}
										statement();
									}
								}
							}
						}
						else {
							error_collect(SEMICN_ERROR, last_word_line);
							goto FORK2;
						}
					}
					else {
						error_collect(SEMICN_ERROR, last_word_line);
						goto FORK1;
					}
				}
			}
		}
	}
	//out << "<ѭ�����>\n";
}

void compiler::foodstep() {
	non_sign_integer();
	//out << "<����>\n";
}

void compiler::condition() {
	return_type item_return_type = NOTHING;
	expression(&item_return_type);
	if (item_return_type != INT) {
		error_collect(CONDITION_ERROR, line);
	}
	if (sym == "LSS") {
		//out << "LSS <\n";
		getsym();
	}
	else if (sym == "LEQ") {
		//out << "LEQ <=\n";
		getsym();
	}
	else if (sym == "GRE") {
		//out << "GRE >\n";
		getsym();
	}
	else if (sym == "GEQ") {
		//out << "GEQ >=\n";
		getsym();
	}
	else if (sym == "NEQ") {
		//out << "NEQ !=\n";
		getsym();
	}
	else if (sym == "EQL") {
		//out << "EQL ==\n";
		getsym();
	}
	item_return_type = NOTHING;
	expression(&item_return_type);
	if (item_return_type != INT) {
		error_collect(CONDITION_ERROR, line);
	}
	//out << "<����>\n";
}

void compiler::expression(return_type* item_return_type) {
	if (sym == "PLUS") {
		//out << "PLUS +\n";
		getsym();
	}
	else if (sym == "MINU") {
		//out << "MINU -\n";
		getsym();
	}
	term(item_return_type);
	while (sym == "PLUS" || sym == "MINU") {
		if (sym == "PLUS") {
			//out << "PLUS +\n";
			getsym();
		}
		else if (sym == "MINU") {
			//out << "MINU -\n";
			getsym();
		}
		term(item_return_type);
		*item_return_type = INT;	// only one �ַ����������з���ֵ�����������
	}
	//out << "<���ʽ>\n";
}

void compiler::term(return_type* item_return_type) {
	factor(item_return_type);
	while (sym == "MULT" || sym == "DIV") {
		if (sym == "MULT") {
			//out << "MULT *\n";
			getsym();
		}
		else if (sym == "DIV") {
			//out << "DIV /\n";
			getsym();
		}
		factor(item_return_type);
		*item_return_type = INT;	// only one �ַ����������з���ֵ�����������
	}
	//out << "<��>\n";
}

void compiler::factor(return_type* item_return_type) {
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		string name = word;
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (!whether_undefine_name(name)) {
			error_collect(UNDEFIEN_NAME_ERROR, line);
			*item_return_type = NOTHING;
		}
		else {
			if (get_return_type(name) == CHAR) {
				*item_return_type = CHAR;	///////////////////////////////////////////////////////////////
			}
			else {
				*item_return_type = INT;
			}
		}
		getsym();
		if (sym == "LBRACK") {
			//out << "LBRACK [\n";
			getsym();
			return_type item_return_type = NOTHING;
			expression(&item_return_type);
			if (item_return_type != INT) {
				error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
			}
			if (sym == "RBRACK") {
				//out << "RBRACK ]\n";
				getsym();
			factor1:
				if (sym == "LBRACK") {
					//out << "LBRACK [\n";
					getsym();
					return_type item_return_type = NOTHING;
					expression(&item_return_type);
					if (item_return_type != INT) {
						error_collect(ARRAY_INDEX_INTEGER_ERROR, line);
					}
					if (sym == "RBRACK") {
						//out << "RBRACK ]\n";
						getsym();
					}
					else {
						error_collect(RBRACK_ERROR, last_word_line);
					}
				}
			}
			else {
				error_collect(RBRACK_ERROR, last_word_line);
				goto factor1;
			}
		}
		if (sym == "LPARENT") {
			have_return_use(name);
		}
	}
	else if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		*item_return_type = INT;
		getsym();
		return_type item_return_type = NOTHING;
		expression(&item_return_type);
		if (sym == "RPARENT") {
			//out << "RPARENT )\n";
			getsym();
		}
		else {
			error_collect(RPARENT_ERROR, last_word_line);
		}
	}
	else if (sym == "MULT") {
		//out << "MULT *\n";
		*item_return_type = CHAR;
		getsym();
	}
	else if (sym == "DIV") {
		//out << "DIV /\n";
		*item_return_type = CHAR;
		getsym();
	}
	else if (sym == "CHARCON") {
		//out << "CHARCON " << word << "\n";
		*item_return_type = CHAR;
		getsym();
	}
	else {
		*item_return_type = INT;
		integer();
	}
	//out << "<����>\n";
}

void compiler::have_return_use(string name) {
	if (sym == "LPARENT") {
		//out << "LPARENT (\n";
		getsym();
		if (sym == "SEMICN" || sym == "COMMA") {	// ���� c(;;;;; ��ɵ�ƴ�
			error_collect(RPARENT_ERROR, last_word_line);
		}
		else {
			value_para(name);
			if (sym == "RPARENT") {
				//out << "RPARENT )\n";
				getsym();
			}
			else {
				error_collect(RPARENT_ERROR, last_word_line);
			}
		}
	}
	//out << "<�з���ֵ�����������>\n";
}

void compiler::value_para(string name) {
	whether_use_func_para_num_right = true;
	whether_use_func_para_type_right = true;

	int now_func_symbol_num = 0;
	int now_func_para_num_expected = 0;
	for (int i = 0; i < symbol_num; i++) {
		if ((*symbol_list[i]).name == name) {
			now_func_symbol_num = i;
			now_func_para_num_expected = (*symbol_list[i]).para_num;
			break;
		}
	}

	int now_func_para_num = 0;

	if (sym != "RPARENT") {
		return_type item_return_type = NOTHING;
		expression(&item_return_type);

		if ((*symbol_list[now_func_symbol_num]).para_list[now_func_para_num] != item_return_type) {
			whether_use_func_para_type_right = false;
		}
		now_func_para_num++;

		while (sym == "COMMA") {
			//out << "COMMA ,\n";
			getsym();
			return_type item_return_type = NOTHING;
			expression(&item_return_type);
			if ((*symbol_list[now_func_symbol_num]).para_list[now_func_para_num] != item_return_type) {
				whether_use_func_para_type_right = false;
			}
			now_func_para_num++;
		}
		if (now_func_para_num_expected != now_func_para_num) {
			whether_use_func_para_num_right = false;
		}

		if (!whether_use_func_para_num_right) {
			error_collect(FUNC_PARA_NUM_ERROR, line);
		}
		if (whether_use_func_para_num_right && (!whether_use_func_para_type_right)) {
			error_collect(FUNC_PARA_TYPE_ERROR, line);
		}
	}
	else {		/////////////////// shit();
		if (now_func_para_num_expected != 0) {
			error_collect(FUNC_PARA_NUM_ERROR, line);
		}
	}
	//out << "<ֵ������>\n";
}

void compiler::analysis_first_letter() {
	if (sym == "INTTK") {
		//out << "INTTK " << word << "\n";
		sym2 = sym;
		getsym();
	}
	else if (sym == "CHARTK") {
		//out << "CHARTK " << word << "\n";
		sym2 = sym;
		getsym();
	}
	if (sym == "IDENFR") {
		//out << "IDENFR " << word << "\n";
		getsym();
	}
}
*/