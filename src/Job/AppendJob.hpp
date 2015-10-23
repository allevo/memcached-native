#ifndef NODEJS_MEMCACHED_JOB_APPEND_H
#define NODEJS_MEMCACHED_JOB_APPEND_H

#include "Base.hpp"

namespace MemcachedNative {


class AppendJob : public JobBase {
public:
	explicit AppendJob(Callback* callback_, char* key_, char* value_)
		: JobBase(callback_), key(key_), value(value_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %s (%zu)\n", "AppendJob execute", key, strlen(key), value, strlen(value));
		rc = memcached_append(mem, key, strlen(key), value, strlen(value), 0, (uint32_t)0);
	}

	virtual ~AppendJob() {
		this->debug && printf("%s\n", "AppendJob deconstructor");
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
