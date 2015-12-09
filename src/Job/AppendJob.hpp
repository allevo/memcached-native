#ifndef NODEJS_MEMCACHED_JOB_APPEND_H
#define NODEJS_MEMCACHED_JOB_APPEND_H

#include "Base.hpp"

namespace MemcachedNative {


class AppendJob : public JobBase {
public:
	explicit AppendJob(Callback* callback_, const string key_, const string value_)
		: JobBase(callback_), key(key_), value(value_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %s (%zu): %s (%zu)\n", "AppendJob execute", key.c_str(), key.size(), value.c_str(), value.size());
		rc = memcached_append(mem, key.c_str(), key.size(), value.c_str(), value.size(), 0, (uint32_t)0);
	}

	virtual ~AppendJob() {
		this->debug && printf("%s\n", "AppendJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	const string key;
	const string value;
};

};


#endif
