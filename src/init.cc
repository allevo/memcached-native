
#include <v8.h>
#include <nan.h>

#include "client.hpp"

#define ADD_CONSTANT(exports, constantKey, constantValue) \
	Nan::ForceSet(exports,                                \
		Nan::New(constantKey).ToLocalChecked(),           \
		Nan::New(constantValue),                          \
		v8::ReadOnly                                      \
	);                                                    \
	// END ADD_CONSTANT

void addConstant(v8::Local<v8::Object> exports) {
	ADD_CONSTANT(exports, "MEMCACHED_SUCCESS", MEMCACHED_SUCCESS);
	ADD_CONSTANT(exports, "MEMCACHED_FAILURE", MEMCACHED_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_HOST_LOOKUP_FAILURE", MEMCACHED_HOST_LOOKUP_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_CONNECTION_FAILURE", MEMCACHED_CONNECTION_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_CONNECTION_BIND_FAILURE", MEMCACHED_CONNECTION_BIND_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_WRITE_FAILURE", MEMCACHED_WRITE_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_READ_FAILURE", MEMCACHED_READ_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_UNKNOWN_READ_FAILURE", MEMCACHED_UNKNOWN_READ_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_PROTOCOL_ERROR", MEMCACHED_PROTOCOL_ERROR);
	ADD_CONSTANT(exports, "MEMCACHED_CLIENT_ERROR", MEMCACHED_CLIENT_ERROR);
	ADD_CONSTANT(exports, "MEMCACHED_SERVER_ERROR", MEMCACHED_SERVER_ERROR);
	ADD_CONSTANT(exports, "MEMCACHED_ERROR", MEMCACHED_ERROR);
	ADD_CONSTANT(exports, "MEMCACHED_DATA_EXISTS", MEMCACHED_DATA_EXISTS);
	ADD_CONSTANT(exports, "MEMCACHED_DATA_DOES_NOT_EXIST", MEMCACHED_DATA_DOES_NOT_EXIST);
	ADD_CONSTANT(exports, "MEMCACHED_NOTSTORED", MEMCACHED_NOTSTORED);
	ADD_CONSTANT(exports, "MEMCACHED_STORED", MEMCACHED_STORED);
	ADD_CONSTANT(exports, "MEMCACHED_NOTFOUND", MEMCACHED_NOTFOUND);
	ADD_CONSTANT(exports, "MEMCACHED_MEMORY_ALLOCATION_FAILURE", MEMCACHED_MEMORY_ALLOCATION_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_PARTIAL_READ", MEMCACHED_PARTIAL_READ);
	ADD_CONSTANT(exports, "MEMCACHED_SOME_ERRORS", MEMCACHED_SOME_ERRORS);
	ADD_CONSTANT(exports, "MEMCACHED_NO_SERVERS", MEMCACHED_NO_SERVERS);
	ADD_CONSTANT(exports, "MEMCACHED_END", MEMCACHED_END);
	ADD_CONSTANT(exports, "MEMCACHED_DELETED", MEMCACHED_DELETED);
	ADD_CONSTANT(exports, "MEMCACHED_VALUE", MEMCACHED_VALUE);
	ADD_CONSTANT(exports, "MEMCACHED_STAT", MEMCACHED_STAT);
	ADD_CONSTANT(exports, "MEMCACHED_ITEM", MEMCACHED_ITEM);
	ADD_CONSTANT(exports, "MEMCACHED_ERRNO", MEMCACHED_ERRNO);
	ADD_CONSTANT(exports, "MEMCACHED_FAIL_UNIX_SOCKET", MEMCACHED_FAIL_UNIX_SOCKET);
	ADD_CONSTANT(exports, "MEMCACHED_NOT_SUPPORTED", MEMCACHED_NOT_SUPPORTED);
	ADD_CONSTANT(exports, "MEMCACHED_NO_KEY_PROVIDED", MEMCACHED_NO_KEY_PROVIDED);
	ADD_CONSTANT(exports, "MEMCACHED_FETCH_NOTFINISHED", MEMCACHED_FETCH_NOTFINISHED);
	ADD_CONSTANT(exports, "MEMCACHED_TIMEOUT", MEMCACHED_TIMEOUT);
	ADD_CONSTANT(exports, "MEMCACHED_BUFFERED", MEMCACHED_BUFFERED);
	ADD_CONSTANT(exports, "MEMCACHED_BAD_KEY_PROVIDED", MEMCACHED_BAD_KEY_PROVIDED);
	ADD_CONSTANT(exports, "MEMCACHED_INVALID_HOST_PROTOCOL", MEMCACHED_INVALID_HOST_PROTOCOL);
	ADD_CONSTANT(exports, "MEMCACHED_SERVER_MARKED_DEAD", MEMCACHED_SERVER_MARKED_DEAD);
	ADD_CONSTANT(exports, "MEMCACHED_UNKNOWN_STAT_KEY", MEMCACHED_UNKNOWN_STAT_KEY);
	ADD_CONSTANT(exports, "MEMCACHED_E2BIG", MEMCACHED_E2BIG);
	ADD_CONSTANT(exports, "MEMCACHED_INVALID_ARGUMENTS", MEMCACHED_INVALID_ARGUMENTS);
	ADD_CONSTANT(exports, "MEMCACHED_KEY_TOO_BIG", MEMCACHED_KEY_TOO_BIG);
	ADD_CONSTANT(exports, "MEMCACHED_AUTH_PROBLEM", MEMCACHED_AUTH_PROBLEM);
	ADD_CONSTANT(exports, "MEMCACHED_AUTH_FAILURE", MEMCACHED_AUTH_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_AUTH_CONTINUE", MEMCACHED_AUTH_CONTINUE);
	ADD_CONSTANT(exports, "MEMCACHED_PARSE_ERROR", MEMCACHED_PARSE_ERROR);
	ADD_CONSTANT(exports, "MEMCACHED_PARSE_USER_ERROR", MEMCACHED_PARSE_USER_ERROR);
	ADD_CONSTANT(exports, "MEMCACHED_DEPRECATED", MEMCACHED_DEPRECATED);
	ADD_CONSTANT(exports, "MEMCACHED_IN_PROGRESS", MEMCACHED_IN_PROGRESS);
	ADD_CONSTANT(exports, "MEMCACHED_SERVER_TEMPORARILY_DISABLED", MEMCACHED_SERVER_TEMPORARILY_DISABLED);
	ADD_CONSTANT(exports, "MEMCACHED_SERVER_MEMORY_ALLOCATION_FAILURE", MEMCACHED_SERVER_MEMORY_ALLOCATION_FAILURE);
	ADD_CONSTANT(exports, "MEMCACHED_MAXIMUM_RETURN", MEMCACHED_MAXIMUM_RETURN);
	ADD_CONSTANT(exports, "MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE", MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE);
}

void Strerror(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	memcached_return_t rc = (memcached_return_t) ( (int) info[0]->NumberValue() );
	info.GetReturnValue().Set(Nan::New(memcached_strerror(NULL, rc)).ToLocalChecked());
}

void Failed(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	memcached_return_t rc = (memcached_return_t) ( (int) info[0]->NumberValue() );
	info.GetReturnValue().Set(Nan::New(memcached_failed(rc)));
}

void Success(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	memcached_return_t rc = (memcached_return_t) ( (int) info[0]->NumberValue() );
	info.GetReturnValue().Set(Nan::New(memcached_success(rc)));
}

void Fatal(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	memcached_return_t rc = (memcached_return_t) ( (int) info[0]->NumberValue() );
	info.GetReturnValue().Set(Nan::New(memcached_fatal(rc)));
}

void CheckConfiguration(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	v8::String::Utf8Value param0(info[0]->ToString());
	std::string param0String = std::string(*param0);
	char* conf = (char*) malloc(param0String.length() * sizeof(char));
	strcpy(conf, param0String.c_str());

	char errorBuffer[2048];
	memcached_return_t rc = libmemcached_check_configuration(conf, strlen(conf), errorBuffer, sizeof(errorBuffer));
	if (!memcached_success(rc)) {
		Nan::ThrowError(errorBuffer);
	}
	info.GetReturnValue().Set(Nan::Undefined());
}

void LibVersion(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	info.GetReturnValue().Set(Nan::New(memcached_lib_version()).ToLocalChecked());
}

void addUtilsFunction(v8::Local<v8::Object> exports) {
	exports->Set(
		Nan::New("strerror").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Strerror)->GetFunction()
	);
	exports->Set(
		Nan::New("isFailed").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Failed)->GetFunction()
	);
	exports->Set(
		Nan::New("isSuccess").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Success)->GetFunction()
	);
	exports->Set(
		Nan::New("isFatal").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Fatal)->GetFunction()
	);
	exports->Set(
		Nan::New("check_configuration").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(CheckConfiguration)->GetFunction()
	);
	exports->Set(
		Nan::New("lib_version").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(LibVersion)->GetFunction()
	);
}

void Init(v8::Local<v8::Object> exports) {
	MemcachedNative::Client::Init(exports);
	addConstant(exports);
	addUtilsFunction(exports);
}

NODE_MODULE(memcachedNative, Init)
