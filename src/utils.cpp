
#include <string>
#include <cstring>

#include "utils.hpp"

char* getCharsFromParam(v8::Local<v8::String> param) {
	v8::String::Utf8Value param0(param);
	std::string param0String = std::string(*param0);
	char* value = new char[param0String.length()];
	strcpy(value, param0String.c_str());
	return value;
}
