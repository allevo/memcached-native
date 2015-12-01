#ifndef NODEJS_MEMCACHED_JOB_DELETE_H
#define NODEJS_MEMCACHED_JOB_DELETE_H

#include "Base.hpp"

namespace MemcachedNative {

class DeleteJob : public JobBase {
public:
	explicit DeleteJob(Callback* callback_, const string key_, uint32_t expiration_time_)
		: JobBase(callback_), key(key_), expiration_time(expiration_time_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %u\n", "DeleteJob execute", key.c_str(), key.size(), expiration_time);
		rc = memcached_delete(mem, key.c_str(), key.size(), expiration_time);
	}

	virtual ~DeleteJob() {
		this->debug && printf("%s\n", "DeleteJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	const string key;
	uint32_t expiration_time;
};


};


#endif
