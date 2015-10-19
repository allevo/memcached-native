#ifndef NODEJS_MEMCACHED_JOB_EXIST_H
#define NODEJS_MEMCACHED_JOB_EXIST_H

#include "Base.hpp"

namespace MemcachedNative {

class ExistJob : public JobBase {
public:
	explicit ExistJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_)
		: JobBase(client_, callback_), key(key_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu)\n", "ExistJob execute", key, strlen(key));
		rc = memcached_exist(mem, key, strlen(key));
	}

	virtual ~ExistJob() {
		this->debug && printf("%s\n", "ExistJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		return Nan::New(memcached_success(rc));
	}

	virtual Local<Value> getError() {
		if (memcached_success(rc) || rc == MEMCACHED_NOTFOUND) {
			return Nan::Undefined();
		}
		return JobBase::getError();
	}

private:
	char* key;
};

};


#endif
