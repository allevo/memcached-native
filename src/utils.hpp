#ifndef NODEJS_MEMCACHED_CHECK_H
#define NODEJS_MEMCACHED_CHECK_H

#include <v8.h>

#define CHECK_ARGUMENT_LENGTH(info, n, nStr) 						\
	if (info.Length() != n) {										\
		Nan::ThrowTypeError("Wrong number of arguments" nStr);		\
		return;														\
	}
	// END CHECK_ARGUMENT_LENGTH

#define CHECK_N_ARGS_IS_A_STRING(info, n, nStr)						\
	if (!info[n]->IsString()) {										\
		Nan::ThrowTypeError(nStr "° argument should be a string");	\
		return;														\
	}																\
	// END CHECK_N_ARGS_IS_A_STRING

#define CHECK_N_ARGS_IS_A_STRING_OR_NUMBER(info, n, nStr)						\
	if (!info[n]->IsString() && !info[n]->IsNumber()) {							\
		Nan::ThrowTypeError(nStr "° argument should be a string or a number");	\
		return;																	\
	}																			\
	// END CHECK_N_ARGS_IS_A_STRING_OR_NUMBER

#define CHECK_N_ARGS_IS_A_NUMBER(info, n, nStr)						\
	if (!info[n]->IsNumber()) {										\
		Nan::ThrowTypeError(nStr "° argument should be a number");	\
		return;														\
	}																\
	// END CHECK_N_ARGS_IS_A_NUMBER

#define CHECK_N_ARGS_IS_A_FUNCTION(info, n, nStr)						\
	if (!info[n]->IsFunction()) {										\
		Nan::ThrowTypeError(nStr "° argument should be a function");	\
		return;															\
	}																	\
	// END CHECK_N_ARGS_IS_A_FUNCTION

#define CHECK_N_ARGS_IS_AN_UNSIGNED_INTEGER(info, n, nStr)						\
	if (!info[n]->IsUint32()) {													\
		Nan::ThrowTypeError(nStr "° argument should be an unsigned integer");	\
		return;																	\
	}																			\
	// END CHECK_N_ARGS_IS_AN_UNSIGNED_INTEGER

#define CHECK_N_ARGS_IS_AN_ARRAY(info, n, nStr)						\
	if (!info[n]->IsArray()) {										\
		Nan::ThrowTypeError(nStr "° argument should be an array");	\
		return;														\
	}																\
	// END CHECK_N_ARGS_IS_AN_ARRAY

#define CHECK_N_ARGS_IS_A_BOOLEAN(info, n, nStr)						\
	if (!info[n]->IsBoolean()) {										\
		Nan::ThrowTypeError(nStr "° argument should be a boolean");		\
		return;															\
	}																	\
	// END CHECK_N_ARGS_IS_A_BOOLEAN


char* getCharsFromParam(v8::Local<v8::String> param);


#endif