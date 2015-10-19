#ifndef NODEJS_MEMCACHED_JOB_MGET_AND_FETCH_ALL_H
#define NODEJS_MEMCACHED_JOB_MGET_AND_FETCH_ALL_H

#include "MGetJob.hpp"
#include "FetchResultJob.hpp"

namespace MemcachedNative {


class MGetAndFetchAllJob : public MGetJob, public FetchResultJob {
public:
	explicit MGetAndFetchAllJob(MemcachedNative::Client*& client_, Callback* callback_, char** keys_, size_t number_of_keys_)
		: JobBase(client_, callback_), MGetJob(client_, callback_, keys_, number_of_keys_), FetchResultJob(client_, callback_) { }

	virtual void execute(memcached_st* mem) {
		MGetJob::execute(mem);
		FetchResultJob::execute(mem);
	}

	virtual ~MGetAndFetchAllJob() {
		this->debug && printf("%s\n", "MGetAndFetchAllJob deconstructor");
	}

	virtual Local<Value> getResult() {
		return FetchResultJob::getResult();
	}

	virtual Local<Value> getError() {
		return FetchResultJob::getError();
	}
};

};


#endif
