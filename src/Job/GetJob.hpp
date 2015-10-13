#ifndef NODEJS_MEMCACHED_JOB_GET_H
#define NODEJS_MEMCACHED_JOB_GET_H

#include "Base.hpp"

namespace MemcachedNative {

class GetJob : public JobBase {
public:
	explicit GetJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_)
		: JobBase(client_, callback_), key(key_), value(NULL) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s\n", "GetJob execute");

		uint32_t flags = 0;
		size_t value_length= 0;
		value = memcached_get(memcached, key, strlen(key), &value_length, &flags, &rc);

		printf("%s %s %d %zu\n", "GetJob executed", value, flags, value_length);
	}

	virtual ~GetJob() {
		printf("%s\n", "GetJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		if (value == NULL) {
			return Nan::Undefined();
		}
		return Nan::New(value).ToLocalChecked();
	}

private:
	char* key;
	char* value;
};

};


#endif
