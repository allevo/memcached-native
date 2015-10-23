#ifndef NODEJS_MEMCACHED_JOB_PREPEND_H
#define NODEJS_MEMCACHED_JOB_PREPEND_H

#include "Base.hpp"

namespace MemcachedNative {


class PrependJob : public JobBase {
public:
	explicit PrependJob(Callback* callback_, char* key_, char* value_)
		: JobBase(callback_), key(key_), value(value_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %s (%zu)\n", "PrependJob execute", key, strlen(key), value, strlen(value));
		rc = memcached_prepend(mem, key, strlen(key), value, strlen(value), 0, (uint32_t)0);
	}

	virtual ~PrependJob() {
		this->debug && printf("%s\n", "PrependJob deconstructor");
		delete key;
		delete value;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	char* value;
};

};


#endif
