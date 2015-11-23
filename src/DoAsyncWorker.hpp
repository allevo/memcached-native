#ifndef NODEJS_MEMCACHED_DO_ASYNC_WORKER_H
#define NODEJS_MEMCACHED_DO_ASYNC_WORKER_H


#include <nan.h>

#include <libmemcached/memcached.h>
#include <libmemcached/util.h>

#include "Job/Base.hpp"

namespace MemcachedNative {

class DoAsyncWorker: public Nan::AsyncWorker {
private:
	memcached_pool_st* pool;
	JobBase* job;
	memcached_return_t rc;

public:
	DoAsyncWorker(memcached_pool_st* _pool, JobBase* _job, Callback* callback) :
		Nan::AsyncWorker(callback), pool(_pool), job(_job)
	{ }

	void Execute() {
		struct timespec tSpec = { time(0) + 10, 0 };
		memcached_st* mClient = memcached_pool_fetch(pool, &tSpec, &rc);
		if (memcached_success(rc)) {
			job->execute(mClient);
			memcached_pool_push(pool, mClient);
		}
	}

	void HandleOKCallback () {
		Nan::HandleScope scope;

		v8::Local<v8::Value> argv[2] = {
			job->getError(),
			job->getResult()
		};
		callback->Call(2, argv);
	}
};

}

#endif
