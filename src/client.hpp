#ifndef CLIENT_H
#define CLIENT_H

#include <nan.h>
#include <set>
#include <libmemcached/memcached.h>

class Job;

namespace MemcachedNative {




class Client : public Nan::ObjectWrap {
public:
	static void Init(v8::Local<v8::Object> exports);

private:
	explicit Client(const char* config_string);
	~Client();

	static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Start(const Nan::FunctionCallbackInfo<v8::Value>& info);
	static void Set(const Nan::FunctionCallbackInfo<v8::Value>& info);

	static Nan::Persistent<v8::Function> constructor;

	memcached_st* client;

public:
	std::set<Job*> jobs;
};

};


#endif
