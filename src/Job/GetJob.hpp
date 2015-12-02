#ifndef NODEJS_MEMCACHED_JOB_GET_H
#define NODEJS_MEMCACHED_JOB_GET_H

#include "Base.hpp"

namespace MemcachedNative {

class GetJob : public JobBase {
public:
	explicit GetJob(Callback* callback_, const string key_)
		: JobBase(callback_), key(key_), value(NULL) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s\n", "GetJob execute");

		uint32_t flags = 0;
		size_t value_length= 0;
		value = memcached_get(mem, key.c_str(), key.size(), &value_length, &flags, &rc);

		this->debug && printf("%s %s %d %zu\n", "GetJob executed", value, flags, value_length);
	}

	virtual ~GetJob() {
		this->debug && printf("%s\n", "GetJob deconstructor");
		// delete value;
	}

	virtual Local<Value> getResult() {
		if (value == NULL) {
			return Nan::Undefined();
		}
		return Nan::New(value).ToLocalChecked();
	}

private:
	const string key;
	char* value;
};

};


#endif
