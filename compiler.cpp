#include "compiler.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include <streambuf>


compiler::compiler() {

	std::ifstream t("testfile.txt");
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	file_in = str;

	//this->out.open("error.txt", ios::out);
	this->mid_code_stream1.open("mid_code1.txt", ios::out);
	this->mid_code_stream2.open("mid_code2.txt", ios::out);

	this->mips_code_stream.open("mips.txt", ios::out);
}

void compiler::start() {
	this->program();
	this->error_output();

	this->printmidcode1();
	this->optimize_mid();
	this->printmidcode2();

	this->Deal_with_mips();
	this->optimize_mips();
	this->print_mipscode();

	this->mid_code_stream1.close();
	this->mid_code_stream2.close();

	this->mips_code_stream.close();
}

/*
#include "compiler.h"

compiler::compiler() {
	this->in.open("testfile.txt", ios::in);
	this->out.open("error.txt", ios::out);
}

void compiler::start() {
	this->program();
	this->error_output();
	this->in.close();
	this->out.close();
}


*/