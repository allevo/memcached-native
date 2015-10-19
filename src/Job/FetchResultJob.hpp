#ifndef NODEJS_MEMCACHED_JOB_FETCH_RESULT_H
#define NODEJS_MEMCACHED_JOB_FETCH_RESULT_H

#include <list>
using std::list;

#include "Base.hpp"

namespace MemcachedNative {


class FetchResultJob : public JobBase {
public:
	explicit FetchResultJob(MemcachedNative::Client*& client_, Callback* callback_)
		: JobBase(client_, callback_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s\n", "FetchResultJob execute");
		memcached_result_st* res;
		while(1) {
			res = memcached_fetch_result(memcached, NULL, &rc);
			if (res == NULL) break;
			results.push_back(res);
		}
	}

	virtual ~FetchResultJob() {
		printf("%s\n", "FetchResultJob deconstructor");
	}

	virtual Local<Value> getError() {
		return Nan::Undefined();
	}

	virtual Local<Value> getResult() {
		v8::Local<v8::Object> result = Nan::New<v8::Object>();
		for (list<memcached_result_st*>::iterator ii = results.begin(); ii != results.end(); ii++) {
			memcached_result_st* current = *ii;
			v8::Local<v8::Object> element = Nan::New<v8::Object>();
			element->Set(Nan::New("key").ToLocalChecked(), Nan::New(memcached_result_key_value(current)).ToLocalChecked());
			element->Set(Nan::New("value").ToLocalChecked(), Nan::New(memcached_result_value(current)).ToLocalChecked());
			element->Set(Nan::New("cas").ToLocalChecked(), Nan::New((uint32_t) memcached_result_cas(current)));

			result->Set(Nan::New(memcached_result_key_value(current)).ToLocalChecked(), element);
		}

		return result;
	}

private:
	list<memcached_result_st*> results;
};

};


#endif
