#ifndef NODEJS_MEMCACHED_JOB_TOUCH_H
#define NODEJS_MEMCACHED_JOB_TOUCH_H

#include "Base.hpp"

namespace MemcachedNative {

class TouchJob : public JobBase {
public:
	explicit TouchJob(Callback* callback_, const string key_, time_t ttl_)
		: JobBase(callback_), key(key_), ttl(ttl_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu) [%ld s]\n", "TouchJob execute", key.c_str(), key.size(), (long int) time(0) + ttl);

		rc = memcached_touch(mem, key.c_str(), key.size(), time(0) + ttl);
	}

	virtual ~TouchJob() {
		this->debug && printf("%s\n", "TouchJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	const string key;
	time_t ttl;
};

};


#endif
