#ifndef NODEJS_MEMCACHED_JOB_CAS_H
#define NODEJS_MEMCACHED_JOB_CAS_H

#include "Base.hpp"

namespace MemcachedNative {


class CasJob : public JobBase {
public:
	explicit CasJob(Callback* callback_, const string key_, const string value_, time_t expirationTime_, uint64_t cas_)
		: JobBase(callback_), key(key_), value(value_), cas(cas_), expirationTime(expirationTime_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %s (%zu) %" PRIu64 "\n", "CasJob execute", key.c_str(), key.size(), value.c_str(), value.size(), cas);
		rc = memcached_cas(mem, key.c_str(), key.size(), value.c_str(), value.size(), expirationTime, (uint32_t)0, cas);
	}

	virtual ~CasJob() {
		this->debug && printf("%s\n", "CasJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	const string key;
	const string value;
	uint64_t cas;
	time_t expirationTime;
};

};


#endif
