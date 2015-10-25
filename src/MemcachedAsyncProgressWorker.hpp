#ifndef NODEJS_MEMCACHED_ASYNC_PROGRESS_WORKER_H
#define NODEJS_MEMCACHED_ASYNC_PROGRESS_WORKER_H

#include <unistd.h>

#include <nan.h>
#include <set>
#include <libmemcached/memcached.h>

#include "Job/Base.hpp"
#include "utils.hpp"

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
		debug(true),
		config_string(config_string_)
	{
		debug && printf("%s\n", "CREATE MemcachedAsyncProgressWorker");
	}

	virtual void Execute(const ExecutionProgress& progress) {
		printf("%s\n", "DD");
		debug && printf("%s: %s (%lu) %s\n", "Execute MemcachedAsyncProgressWorker", config_string, strlen(config_string), client->isRunning ? "y" : "n");

		// memcached_st* memcacheClient = memcached(config_string, strlen(config_string));
		debug && printf("%s %p\n", "Created memcached client at", client->memcacheClient);
		while(client->isRunning) {
			usleep(100);
			debug && printf("%s\n", "MemcachedAsyncProgressWorker Execute locking");
			uv_mutex_lock(&(client->mutex_handle));
			debug && printf("%s\n", "MemcachedAsyncProgressWorker Execute locked");
			debug && printf("%s %lu\n", "Jobs count: ", client->jobs.size());
			for(std::set<JobBase*>::iterator ii = client->jobs.begin(); ii != client->jobs.end(); ii++) {
				JobBase* current = *ii;
				debug && printf("%s %p\n", "Current job is placed at", current);

				if (current->isDone) continue;

				current->execute(client->memcacheClient);
				current->isDone = true;
			}
			if (client->jobs.size() != 0) {
				progress.Send(NULL, 0);
			}
			debug && printf("%s\n", "MemcachedAsyncProgressWorker Execute unlocking");
			uv_mutex_unlock(&(client->mutex_handle));
			debug && printf("%s\n", "MemcachedAsyncProgressWorker Execute unlocked");
		}

		// Is it safe here?
		memcached_free(client->memcacheClient);

		// usleep(1000);
	}

	virtual void HandleProgressCallback(const char *data, size_t size) {
		// printf("%s\n", "HandleProgressCallback MemcachedAsyncProgressWorker");
	    Nan::HandleScope scope;

		std::set<JobBase*>::iterator ii = client->jobs.begin();
		for(;ii != client->jobs.end(); ii++) {
			JobBase* current = *ii;
			debug && printf("%s\n", "Current");
			if (!current->isDone) continue;

			debug && printf("%s\n", "Call callback");
			v8::Local<v8::Value> argv[2] = {
				current->getError(),
				current->getResult(),
			};

			debug && printf("%s\n", "Calling callback");
			current->callback->Call(2, argv);
			debug && printf("%s\n", "Called callback");

			debug && printf("%s\n", "MemcachedAsyncProgressWorker HandleProgressCallback locking");
		    uv_mutex_lock(&(client->mutex_handle));
			debug && printf("%s\n", "MemcachedAsyncProgressWorker HandleProgressCallback locked");

			debug && printf("%s\n", "Deleting job");
			delete current;
			debug && printf("%s\n", "Deleted job");
			debug && printf("%s\n", "Erasing job");
			client->jobs.erase(ii);
			debug && printf("%s\n", "Erased job");

			debug && printf("%s\n", "MemcachedAsyncProgressWorker HandleProgressCallback unlocking");
			uv_mutex_unlock(&(client->mutex_handle));
			debug && printf("%s\n", "MemcachedAsyncProgressWorker HandleProgressCallback unlocked");
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
