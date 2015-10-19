#ifndef NODEJS_MEMCACHED_JOB_CAS_H
#define NODEJS_MEMCACHED_JOB_CAS_H

#include "Base.hpp"

namespace MemcachedNative {


class CasJob : public JobBase {
public:
	explicit CasJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, char* value_, time_t expirationTime_, uint32_t cas_)
		: JobBase(client_, callback_), key(key_), value(value_), cas(cas_), expirationTime(expirationTime_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s %s (%zu): %s (%zu) %u\n", "CasJob execute", key, strlen(key), value, strlen(value), cas);
		rc = memcached_cas(memcached, key, strlen(key), value, strlen(value), expirationTime, (uint32_t)0, (uint64_t) cas);
	}

	virtual ~CasJob() {
		printf("%s\n", "CasJob deconstructor");
		delete key;
		delete value;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	char* value;
	uint32_t cas;
	time_t expirationTime;
};

};


#endif
