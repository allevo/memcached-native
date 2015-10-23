#ifndef NODEJS_MEMCACHED_ASYNC_PROGRESS_WORKER_H
#define NODEJS_MEMCACHED_ASYNC_PROGRESS_WORKER_H

#include <unistd.h>

#include <nan.h>
#include <set>
#include <libmemcached/memcached.h>

#include "Job/Base.hpp"

using v8::Local;
using v8::Value;
using v8::Exception;
using v8::Object;

using Nan::AsyncProgressWorker;
using Nan::Callback;

namespace MemcachedNative {

class Client;

class MemcachedAsyncProgressWorker : public AsyncProgressWorker {
public:
	explicit MemcachedAsyncProgressWorker(Client* client_, Callback* callback_, const char* config_string_) :
		AsyncProgressWorker(callback_),
		client(client_),
		debug(false),
		config_string(config_string_)
	{
		debug && printf("%s\n", "CREATE MemcachedAsyncProgressWorker");
	}

	virtual void Execute(const ExecutionProgress& progress) {
		debug && printf("%s\n", "Execute MemcachedAsyncProgressWorker");

		memcached_st* memcacheClient = memcached(config_string, strlen(config_string));
		while(client->isRunning) {
			usleep(100);
			for(std::set<JobBase*>::iterator ii = client->jobs.begin(); ii != client->jobs.end(); ii++) {
				JobBase* current = *ii;
				if (current->isDone) continue;

				current->execute(memcacheClient);
				current->isDone = true;
			}
			progress.Send(NULL, 0);
		}

		// Is it safe here?
		memcached_free(memcacheClient);
	}

	virtual void HandleProgressCallback(const char *data, size_t size) {
		// printf("%s\n", "HandleProgressCallback MemcachedAsyncProgressWorker");
	    Nan::HandleScope scope;

		std::set<JobBase*>::iterator ii = client->jobs.begin();
		for(;ii != client->jobs.end(); ii++) {
			JobBase* current = *ii;
			if (!current->isDone) continue;

			v8::Local<v8::Value> argv[2] = {
				current->getError(),
				current->getResult(),
			};

			current->callback->Call(2, argv);

			delete current;
			client->jobs.erase(ii);
		}
	}

	void setCallback(Callback* callback_) {
		callback = callback_;
	}

	void setDebug(bool debug_) {
		debug = debug_;
	}

private:
	Client* client;
	bool debug;
	const char* config_string;
};


};


#endif
