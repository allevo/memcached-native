#ifndef NODEJS_MEMCACHED_CLIENT_H
#define NODEJS_MEMCACHED_CLIENT_H

#include <nan.h>
#include <set>
#include <libmemcached/memcached.h>
#include <libmemcached/util.h>
#include "utils.hpp"

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
	static void Set(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Get(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Touch(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Increment(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Decrement(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Prepend(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Append(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Delete(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Exist(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Replace(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Cas(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void MGet(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void FetchResult(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void MGetAndFetchAll(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Stop(const Nan::FunctionCallbackInfo<v8::Value>& info);

	static void Debug(const Nan::FunctionCallbackInfo<v8::Value>& info);

	static Nan::Persistent<v8::Function> constructor;
public:
	memcached_pool_st* pool;
	bool debug;

	std::set<JobBase*> jobs;
	bool isRunning;

	MemcachedAsyncProgressWorker* progressWorker;
};

};


#endif
