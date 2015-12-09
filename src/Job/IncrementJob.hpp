#ifndef NODEJS_MEMCACHED_JOB_INCREMENT_H
#define NODEJS_MEMCACHED_JOB_INCREMENT_H

#include "Base.hpp"

namespace MemcachedNative {

class IncrementJob : public JobBase {
public:
	explicit IncrementJob(Callback* callback_, const string key_, uint32_t delta_)
		: JobBase(callback_), key(key_), delta(delta_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %d\n", "IncrementJob execute", key.c_str(), key.size(), delta);
		rc = memcached_increment(mem, key.c_str(), key.size(), delta, &finalValue);
	}

	virtual ~IncrementJob() {
		this->debug && printf("%s\n", "IncrementJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::New((uint32_t) finalValue);
	}

private:
	const string key;
	uint32_t delta;
	uint64_t finalValue;
};


};


#endif
