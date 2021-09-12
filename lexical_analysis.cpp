#include "compiler.h"
#include <algorithm>


void compiler::getsym() {
	last_word_line = line;
	
	while (point != EOF) {
		index++;
		point = file_in[index];
		if (isspace(point)) {
			if (point == '\n') {	///////////////////////就很离谱
				line = line + 1;
			}
			continue;
		}
		break;
	}
	switch (point) {

	case ' ': {
		goto end;
	}
	case '\r': {
		goto end;
	}
	case '\t': {
		goto end;
	}
	case '+':
		//out << "PLUS +\n";
		sym = "PLUS";
		goto end;

	case '-':
		//out << "MINU -\n";
		sym = "MINU";
		goto end;

	case '*':
		//out << "MULT *\n";
		sym = "MULT";
		goto end;

	case '/':
		//out << "DIV /\n";
		sym = "DIV";
		goto end;

	case ':':
		//out << "COLON :\n";
		sym = "COLON";
		goto end;

	case ';':
		//out << "SEMICN ;\n";
		sym = "SEMICN";
		goto end;

	case ',':
		//out << "COMMA ,\n";
		sym = "COMMA";
		goto end;

	case '(':
		//out << "LPARENT (\n";
		sym = "LPARENT";
		goto end;

	case ')':
		//out << "RPARENT )\n";
		sym = "RPARENT";
		goto end;

	case '[':
		//out << "LBRACK [\n";
		sym = "LBRACK";
		goto end;

	case ']':
		//out << "RBRACK ]\n";
		sym = "RBRACK";
		goto end;

	case '{':
		//out << "LBRACE {\n";
		sym = "LBRACE";
		goto end;

	case '}':
		//out << "RBRACE }\n";
		sym = "RBRACE";
		goto end;

	default:
		break;
	}

	switch (point) {

	case '<': {
		index++;
		point = file_in[index];
		if (point == '=') {
			//out << "LEQ <=\n";
			sym = "LEQ";
			goto end;
		}
		else {
			index--;
			//out << "LSS <\n";
			sym = "LSS";
			goto end;
		}
	}
	case '>': {
		index++;
		point = file_in[index];
		if (point == '=') {
			//out << "GEQ >=\n";
			sym = "GEQ";
			goto end;
		}
		else {
			index--;
			//out << "GRE >\n";
			sym = "GRE";
			goto end;
		}
	}
	case '!': {
		index++;
		point = file_in[index];
		if (point == '=') {
			//out << "NEQ !=\n";
			sym = "NEQ";
			goto end;
		}
	}
	case '=': {
		index++;
		point = file_in[index];
		if (point == '=') {
			//out << "EQL ==\n";
			sym = "EQL";
			goto end;
		}
		else {
			index--;
			//out << "ASSIGN =\n";
			sym = "ASSIGN";
			goto end;
		}
	}
	default:
		break;
	}

	if (isalpha(point) || point == '_') {
		char temp[1000];
		int note = 0;
		temp[0] = point;
		note = note + 1;
		index++;
		point = file_in[index];
		while (isalpha(point) || isdigit(point) != 0 || point == '_') {
			temp[note] = point;
			note = note + 1;
			index++;
			point = file_in[index];
		}
		index--;
		temp[note] = '\0';

		if (insentive_compare(temp, "const")) {
			//out << "CONSTTK " << temp << "\n";
			word = temp;
			sym = "CONSTTK";
			goto end;
		}
		else if (insentive_compare(temp, "int")) {
			//out << "INTTK " << temp << "\n";
			word = temp;
			sym = "INTTK";
			goto end;
		}
		else if (insentive_compare(temp, "char")) {
			//out << "CHARTK " << temp << "\n";
			word = temp;
			sym = "CHARTK";
			goto end;
		}
		else if (insentive_compare(temp, "void")) {
			//out << "VOIDTK " << temp << "\n";
			word = temp;
			sym = "VOIDTK";
			goto end;
		}
		else if (insentive_compare(temp, "main")) {
			//out << "MAINTK " << temp << "\n";
			word = temp;
			sym = "MAINTK";
			goto end;
		}
		else if (insentive_compare(temp, "if")) {
			//out << "IFTK " << temp << "\n";
			word = temp;
			sym = "IFTK";
			goto end;
		}
		else if (insentive_compare(temp, "else")) {
			//out << "ELSETK " << temp << "\n";
			word = temp;
			sym = "ELSETK";
			goto end;
		}
		else if (insentive_compare(temp, "switch")) {
			//out << "SWITCHTK " << temp << "\n";
			word = temp;
			sym = "SWITCHTK";
			goto end;
		}
		else if (insentive_compare(temp, "case")) {
			//out << "CASETK " << temp << "\n";
			word = temp;
			sym = "CASETK";
			goto end;
		}
		else if (insentive_compare(temp, "while")) {
			//out << "WHILETK " << temp << "\n";
			word = temp;
			sym = "WHILETK";
			goto end;
		}
		else if (insentive_compare(temp, "for")) {
			//out << "FORTK " << temp << "\n";
			word = temp;
			sym = "FORTK";
			goto end;
		}
		else if (insentive_compare(temp, "scanf")) {
			//out << "SCANFTK " << temp << "\n";
			word = temp;
			sym = "SCANFTK";
			goto end;
		}
		else if (insentive_compare(temp, "printf")) {
			//out << "PRINTFTK " << temp << "\n";
			word = temp;
			sym = "PRINTFTK";
			goto end;
		}
		else if (insentive_compare(temp, "return")) {
			//out << "RETURNTK " << temp << "\n";
			word = temp;
			sym = "RETURNTK";
			goto end;
		}
		else if (insentive_compare(temp, "default")) {
			//out << "DEFAULTTK " << temp << "\n";
			word = temp;
			sym = "DEFAULTTK";
			goto end;
		}
		else {
			word = temp;
			sym = "IDENFR";
			goto end;
		}
	}

	if (point == '"') {
		char temp[1000];
		int note = 0;
		index++;
		point = file_in[index];
		while (point != '"' && (point == 32 || point == 33 || (point >= 35 && point <= 126))) { //////////////////////
			temp[note] = point;
			note = note + 1;
			index++;
			point = file_in[index];
		}
		if (point == '"') {
			if (note != 0) {
				temp[note] = '\0';
				word = temp;
				sym = "STRCON";
				goto end;
			}
			else {
				sym = "STRCON";
				error_collect(ILLGEAL_CHAR_ERROR, line);
				goto end;
			}
		}
		else {
			sym = "STRCON";
			error_collect(ILLGEAL_CHAR_ERROR, line);
			goto end;
		}
	}

	if (isdigit(point)) {
		char temp[1000];
		int note = 0;
		temp[note] = point;
		note = note + 1;
		index++;
		point = file_in[index];
		while (isdigit(point)) {
			temp[note] = point;
			note = note + 1;
			index++;
			point = file_in[index];
		}
		temp[note] = '\0';
		word = temp;
		sym = "INTCON";
		index--;
		goto end;
	}
	/*
	if (point == '\'') {
		point = in.get();
		if (point != '\'') {
			word = point; //??????????
			sym = "CHARCON"; // ??????????????????????????????????????
			point = in.get();
			goto end;
		}
	}
	*/

	if (point == '\'') {
		index++;
		point = file_in[index];
		if (point == '_' || point == '+' || point == '-' || point == '*' || point == '/' ||
			('0' <= point && point <= '9') || ('a' <= point && point <= 'z') || ('A' <= point && point <= 'Z')) {
			char now_char = point;
			index++;
			point = file_in[index];
			if (point == '\'') {
				word = now_char;
				sym = "CHARCON";
				goto end;
			}
		}
		else {
			if (point == '\'') {
				sym = "CHARCON";				///////////////////// 乱返回了一个
			}
			else {
				char now_char = point;
				word = now_char;
				sym = "CHARCON";
				index++;
				point = file_in[index];
			}
			error_collect(ILLGEAL_CHAR_ERROR, line);
			goto end;
		}
	}
end:;
}

bool compiler::insentive_compare_char(unsigned char a, unsigned char b) {
	return tolower(a) == tolower(b);
}

bool compiler::insentive_compare(string a, string b) {	///////////////////////////////////////////
	transform(a.begin(), a.end(), a.begin(), ::tolower);
	transform(b.begin(), b.end(), b.begin(), ::tolower);
	if (a == b) {
		return true;
	}
	else {
		return false;
	}

}

