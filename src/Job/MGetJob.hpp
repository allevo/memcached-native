#ifndef NODEJS_MEMCACHED_JOB_MGET_H
#define NODEJS_MEMCACHED_JOB_MGET_H

#include "Base.hpp"

namespace MemcachedNative {


class MGetJob : public virtual JobBase {
public:
	explicit MGetJob(MemcachedNative::Client*& client_, Callback* callback_, char** keys_, size_t number_of_keys_)
		: JobBase(client_, callback_), keys(keys_), number_of_keys(number_of_keys_) { }

	virtual void execute(memcached_st* mem) {
		this->debug && printf("%s %lu keys", "MGetJob execute", number_of_keys);
		size_t* key_length = new size_t[number_of_keys];
		for(size_t i = 0; i < number_of_keys; i++) {
			this->debug && printf(" [%lu] %s", i, keys[i]);
			key_length[i] = strlen(keys[i]);
		}
		this->debug && printf("\n");
		rc = memcached_mget(mem, keys, key_length, number_of_keys);
	}

	virtual ~MGetJob() {
		this->debug && printf("%s\n", "MGetJob deconstructor");
		for(size_t i = 0; i < number_of_keys; i++) {
			delete keys[i];
		}
		delete keys;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char** keys;
	size_t number_of_keys;
};

};


#endif
