#ifndef NODEJS_MEMCACHED_CLIENT_H
#define NODEJS_MEMCACHED_CLIENT_H

#include <nan.h>
#include <set>
#include <libmemcached/memcached.h>

#include "Job/Base.hpp"

namespace MemcachedNative {

class MemcachedAsyncProgressWorker;

class Client : public Nan::ObjectWrap {
public:
	static void Init(v8::Local<v8::Object> exports);

private:
	explicit Client(const char* config_string);
	~Client();

	static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);

	static void Start(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Stop(const Nan::FunctionCallbackInfo<v8::Value>& info);

	static void Set(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Get(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Touch(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Increment(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Decrement(const Nan::FunctionCallbackInfo<v8::Value>& info);

	static Nan::Persistent<v8::Function> constructor;

	MemcachedAsyncProgressWorker* backgroundThread;
public:
	memcached_st* client;
	std::set<JobBase*> jobs;
	bool isRunning;
};

};


#endif
