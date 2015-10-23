#ifndef NODEJS_MEMCACHED_JOB_INCREMENT_H
#define NODEJS_MEMCACHED_JOB_INCREMENT_H

#include "Base.hpp"

namespace MemcachedNative {

class IncrementJob : public JobBase {
public:
	explicit IncrementJob(Callback* callback_, char* key_, uint32_t delta_)
		: JobBase(callback_), key(key_), delta(delta_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %d\n", "IncrementJob execute", key, strlen(key), delta);
		rc = memcached_increment(mem, key, strlen(key), delta, &finalValue);
	}

	virtual ~IncrementJob() {
		this->debug && printf("%s\n", "IncrementJob deconstructor");
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
