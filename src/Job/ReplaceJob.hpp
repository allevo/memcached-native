#ifndef NODEJS_MEMCACHED_JOB_REPLACE_H
#define NODEJS_MEMCACHED_JOB_REPLACE_H

#include "Base.hpp"

namespace MemcachedNative {


class ReplaceJob : public JobBase {
public:
	explicit ReplaceJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, char* value_, time_t ttl_)
		: JobBase(client_, callback_), key(key_), value(value_), ttl(ttl_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s %s (%zu): %s (%zu) [%ld s] \n", "ReplaceJob execute", key, strlen(key), value, strlen(value), (long int) ttl);
		rc = memcached_replace(memcached, key, strlen(key), value, strlen(value), ttl, (uint32_t)0);
	}

	virtual ~ReplaceJob() {
		printf("%s\n", "ReplaceJob deconstructor");
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
