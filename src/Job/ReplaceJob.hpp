#ifndef NODEJS_MEMCACHED_JOB_REPLACE_H
#define NODEJS_MEMCACHED_JOB_REPLACE_H

#include "Base.hpp"

namespace MemcachedNative {


class ReplaceJob : public JobBase {
public:
	explicit ReplaceJob(Callback* callback_, const string key_, const string value_, time_t ttl_)
		: JobBase(callback_), key(key_), value(value_), ttl(ttl_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %s (%zu) [%ld s] \n", "ReplaceJob execute", key.c_str(), key.size(), value.c_str(), value.size(), (long int) ttl);
		rc = memcached_replace(mem, key.c_str(), key.size(), value.c_str(), value.size(), ttl, (uint32_t)0);
	}

	virtual ~ReplaceJob() {
		this->debug && printf("%s\n", "ReplaceJob deconstructor");
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
