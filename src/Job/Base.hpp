#ifndef NODEJS_MEMCACHED_JOB_BASE_H
#define NODEJS_MEMCACHED_JOB_BASE_H

#include <nan.h>
#include <libmemcached/memcached.h>

#include <string>

using std::string;

using v8::Local;
using v8::Value;
using v8::Exception;
using v8::Object;

using Nan::AsyncProgressWorker;
using Nan::Callback;

namespace MemcachedNative {

class JobBase {
public:
	explicit JobBase(Callback* callback_)
		: callback(callback_), isDone(false), debug(false) { }

	virtual ~JobBase() { }

	virtual void execute(memcached_st* memcached) = 0;
	virtual Local<Value> getResult() = 0;

	virtual Local<Value> getError() {
		if (memcached_success(rc)) {
			return Nan::Undefined();
		}
		Local<Value> error = Exception::Error(Nan::New(memcached_strerror(NULL, rc)).ToLocalChecked());
		Local<Object> errorAsObj = error.As<Object>();
		Nan::Set(errorAsObj, Nan::New("errno").ToLocalChecked(), Nan::New((int) rc));
		Nan::Set(errorAsObj, Nan::New("code").ToLocalChecked(), Nan::New(memcached_strerror(NULL, rc)).ToLocalChecked());
		return error;
	}

	virtual void setDebug(bool debug_) {
		debug = debug_;
	}

public:
	Callback* callback;
	bool isDone;

protected:
	memcached_return_t rc;

	bool debug;
};

};


#endif
