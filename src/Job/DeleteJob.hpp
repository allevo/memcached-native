#ifndef NODEJS_MEMCACHED_JOB_DELETE_H
#define NODEJS_MEMCACHED_JOB_DELETE_H

#include "Base.hpp"

namespace MemcachedNative {

class DeleteJob : public JobBase {
public:
	explicit DeleteJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, uint32_t expiration_time_)
		: JobBase(client_, callback_), key(key_), expiration_time(expiration_time_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %u\n", "DeleteJob execute", key, strlen(key), expiration_time);
		rc = memcached_delete(mem, key, strlen(key), expiration_time);
	}

	virtual ~DeleteJob() {
		this->debug && printf("%s\n", "DeleteJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	uint32_t expiration_time;
};


};


#endif
