#ifndef NODEJS_MEMCACHED_JOB_EXIST_H
#define NODEJS_MEMCACHED_JOB_EXIST_H

#include "Base.hpp"

namespace MemcachedNative {

class ExistJob : public JobBase {
public:
	explicit ExistJob(Callback* callback_, const string key_)
		: JobBase(callback_), key(key_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu)\n", "ExistJob execute", key.c_str(), key.size());
		rc = memcached_exist(mem, key.c_str(), key.size());
	}

	virtual ~ExistJob() {
		this->debug && printf("%s\n", "ExistJob deconstructor");
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
	const string key;
};

};


#endif
