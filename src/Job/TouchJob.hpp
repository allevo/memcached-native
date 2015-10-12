#ifndef NODEJS_MEMCACHED_JOB_TOUCH_H
#define NODEJS_MEMCACHED_JOB_TOUCH_H

#include "Base.hpp"

namespace MemcachedNative {

class TouchJob : public JobBase {
public:
	explicit TouchJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, int ttl_)
		: JobBase(client_, callback_), key(key_), ttl(ttl_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s\n", "TouchJob execute");

		rc = memcached_touch(memcached, key, strlen(key), (time_t) ttl);
	}

	virtual ~TouchJob() {
		printf("%s\n", "TouchJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	int ttl;
};

};


#endif