#ifndef NODEJS_MEMCACHED_JOB_MGET_H
#define NODEJS_MEMCACHED_JOB_MGET_H

#include "Base.hpp"

#include <vector>

namespace MemcachedNative {


class MGetJob : public virtual JobBase {
public:
	explicit MGetJob(Callback* callback_, std::vector<string> keys_)
		: JobBase(callback_), keys(keys_) { }

	virtual void execute(memcached_st* mem) {
		size_t number_of_keys = keys.size();
		this->debug && printf("%s %lu keys", "MGetJob execute", number_of_keys);

		char** keys_chars = new char*[number_of_keys];
		size_t* key_length = new size_t[number_of_keys];
		string current = "";
		int i = 0;
		for(std::vector<string>::iterator ii = keys.begin(); ii != keys.end(); ii++) {
			current = *ii;
			this->debug && printf(" %s", current.c_str());
			keys_chars[i] = (char*) current.c_str();
			key_length[i] = current.size();
			i++;
		}
		this->debug && printf("\n");
		rc = memcached_mget(mem, keys_chars, key_length, number_of_keys);
	}

	virtual ~MGetJob() {
		this->debug && printf("%s\n", "MGetJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	std::vector<string> keys;
};

};


#endif
