#ifndef NODEJS_MEMCACHED_JOB_SET_H
#define NODEJS_MEMCACHED_JOB_SET_H

#include "Base.hpp"

namespace MemcachedNative {

class SetJob : public JobBase {
public:
	explicit SetJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, char* value_, time_t ttl_)
		: JobBase(client_, callback_), key(key_), value(value_), ttl(ttl_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s %s (%zu): %s (%zu) [%ld s] \n", "SetJob execute", key, strlen(key), value, strlen(value), (long int) ttl);
		rc = memcached_set(memcached, key, strlen(key), value, strlen(value), ttl, (uint32_t)0);
	}

	virtual ~SetJob() {
		printf("%s\n", "SetJob deconstructor");
		delete key;
		delete value;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	char* value;
	time_t ttl;
};


};


#endif
