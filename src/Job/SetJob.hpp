#ifndef NODEJS_MEMCACHED_JOB_SET_H
#define NODEJS_MEMCACHED_JOB_SET_H

#include "Base.hpp"

namespace MemcachedNative {

class SetJob : public JobBase {
public:
	explicit SetJob(Callback* callback_, const string key_, const string value_, time_t ttl_)
		: JobBase(callback_), key(key_), value(value_), ttl(ttl_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %s (%zu) [%ld s] %s\n", "SetJob execute", key.c_str(), key.size(), value.c_str(), value.size(), (long int) ttl, memcached_strerror(NULL, rc));
		rc = memcached_set(mem, key.c_str(), key.size(), value.c_str(), value.size(), time(0) + ttl, (uint32_t)0);
		this->debug && printf("%s %s (%zu): %s (%zu) [%ld s] %s\n", "SetJob execute", key.c_str(), key.size(), value.c_str(), value.size(), (long int) ttl, memcached_strerror(NULL, rc));
	}

	virtual ~SetJob() {
		this->debug && printf("%s\n", "SetJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	const string key;
	const string value;
	time_t ttl;
};


};


#endif
