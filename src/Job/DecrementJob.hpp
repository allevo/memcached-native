#ifndef NODEJS_MEMCACHED_JOB_DECREMENT_H
#define NODEJS_MEMCACHED_JOB_DECREMENT_H

#include "Base.hpp"

namespace MemcachedNative {

class DecrementJob : public JobBase {
public:
	explicit DecrementJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, uint32_t delta_)
		: JobBase(client_, callback_), key(key_), delta(delta_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %d\n", "DecrementJob execute", key, strlen(key), delta);
		rc = memcached_decrement(mem, key, strlen(key), delta, &finalValue);
	}

	virtual ~DecrementJob() {
		this->debug && printf("%s\n", "DecrementJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		return Nan::New((uint32_t) finalValue);
	}

private:
	char* key;
	uint32_t delta;
	uint64_t finalValue;
};


};


#endif
