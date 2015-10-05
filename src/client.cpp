#include "client.hpp"

#include <unistd.h>

using namespace MemcachedNative;

using v8::Local;
using v8::Value;
using v8::Exception;
using v8::Object;

using Nan::AsyncProgressWorker;
using Nan::Callback;


Nan::Persistent<v8::Function> Client::constructor;

class Job {
public:
	explicit Job(MemcachedNative::Client*& client_, Callback* callback_)
		: client(client_), callback(callback_), isDone(false) { }

	virtual ~Job() { }

	virtual void execute(memcached_st* memcached) = 0;
	virtual Local<Value> getResult() = 0;

	virtual Local<Value> getError() {
		if (memcached_success(rc)) {
			return Nan::Undefined();
		}
		Local<Value> error = Exception::Error(Nan::New(memcached_strerror(NULL, rc)).ToLocalChecked());
		Local<Object> errorAsObj = error.As<Object>();
		Nan::Set(errorAsObj, Nan::New("errno").ToLocalChecked(), Nan::New((int) rc));
		Nan::Set(errorAsObj, Nan::New("code").ToLocalChecked(), Nan::New(memcached_strerror(NULL, rc)).ToLocalChecked());
		return error;
	}

private:
	MemcachedNative::Client* client;

public:
	Callback* callback;
	bool isDone;

protected:
	memcached_return_t rc;
};

class SetJob : public Job {
public:
	explicit SetJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, char* value_, time_t ttl_)
		: Job(client_, callback_), key(key_), value(value_), ttl(ttl_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s %s (%d): %s (%d) [%d s] \n", "SetJob execute", key, strlen(key), value, strlen(value), ttl);
		rc = memcached_set(memcached, key, strlen(key), value, strlen(value), ttl, (uint32_t)0);
	}

	virtual ~SetJob() {
		printf("%s\n", "SetJob deconstructor");
		delete key;
		delete value;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	char* value;
	time_t ttl;
};

class GetJob : public Job {
public:
	explicit GetJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_)
		: Job(client_, callback_), key(key_), value(NULL) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s\n", "GetJob execute");

		uint32_t flags = 0;
		size_t value_length= 0;
		value = memcached_get(memcached, key, strlen(key), &value_length, &flags, &rc);

		printf("%s %s %d %zu\n", "GetJob executed", value, flags, value_length);
	}

	virtual ~GetJob() {
		printf("%s\n", "SetJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		if (value == NULL) {
			return Nan::Undefined();
		}
		return Nan::New(value).ToLocalChecked();
	}

private:
	char* key;
	char* value;
};

class TouchJob : public Job {
public:
	explicit TouchJob(MemcachedNative::Client*& client_, Callback* callback_, char* key_, int ttl_)
		: Job(client_, callback_), key(key_), ttl(ttl_) { }

	virtual void execute(memcached_st* memcached) {
		printf("%s\n", "TouchJob execute");

		rc = memcached_touch(memcached, key, strlen(key), (time_t) ttl);
	}

	virtual ~TouchJob() {
		printf("%s\n", "SetJob deconstructor");
		delete key;
	}

	virtual Local<Value> getResult() {
		return Nan::Undefined();
	}

private:
	char* key;
	int ttl;
};

class MemcachedAsyncProgressWorker : public AsyncProgressWorker {
public:
	explicit MemcachedAsyncProgressWorker(Client* client_, Callback* callback_) :
		AsyncProgressWorker(callback_),
		client(client_)
	{
		printf("%s\n", "CREATE MemcachedAsyncProgressWorker");
	}

	virtual void Execute(const ExecutionProgress& progress) {
		printf("%s\n", "Execute MemcachedAsyncProgressWorker");

		while(client->isRunning) {
			usleep(10 * 1000);
			// printf("%s %zu\n", "whiling...", client->jobs.size());
			for(std::set<Job*>::iterator ii = client->jobs.begin(); ii != client->jobs.end(); ii++) {
				Job* current = *ii;
				if (current->isDone) continue;

				current->execute(client->client);
				current->isDone = true;
			}
			progress.Send(NULL, 0);
		}

		// Is it safe here?
		memcached_free(client->client);
	}

	virtual void HandleProgressCallback(const char *data, size_t size) {
		// printf("%s\n", "HandleProgressCallback MemcachedAsyncProgressWorker");
	    Nan::HandleScope scope;

		std::set<Job*>::iterator ii = client->jobs.begin();
		for(;ii != client->jobs.end(); ii++) {
			Job* current = *ii;
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

private:
	Client* client;
};


Client::Client(const char* config_string)
	: isRunning(false)
{
	this->client = memcached(config_string, strlen(config_string));
}

Client::~Client() { }

void Client::Init(v8::Local<v8::Object> exports) {
	Nan::HandleScope scope;

	// Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("Client").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "start", Start);
	Nan::SetPrototypeMethod(tpl, "set", Set);
	Nan::SetPrototypeMethod(tpl, "get", Get);
	Nan::SetPrototypeMethod(tpl, "stop", Stop);
	Nan::SetPrototypeMethod(tpl, "touch", Touch);

	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Client").ToLocalChecked(), tpl->GetFunction());
}

void Client::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	if (info.IsConstructCall()) {
		v8::String::Utf8Value param1(info[0]->ToString());
		std::string from = std::string(*param1);

		const char* conf = info[0]->IsUndefined() ? "--SERVER=localhost" : from.c_str();

		Client* obj = new Client(conf);
		obj->Wrap(info.This());
		info.GetReturnValue().Set(info.This());
	} else {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = { info[0] };
		v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
		info.GetReturnValue().Set(cons->NewInstance(argc, argv));
	}
}

void Client::Start(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());
	Callback* callback = new Callback(info[0].As<v8::Function>());

	memClient->isRunning = true;

	memClient->backgroundThread = new MemcachedAsyncProgressWorker(memClient, callback);
	Nan::AsyncQueueWorker(memClient->backgroundThread);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Set(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	v8::String::Utf8Value param0(info[0]->ToString());
	std::string param0String = std::string(*param0);
	char* memcached_key = (char*) malloc(param0String.length() * sizeof(char));
	strcpy(memcached_key, param0String.c_str());

	v8::String::Utf8Value param1(info[1]->ToString());
	std::string param1String = std::string(*param1);
	char* memcached_value = (char*) malloc(param1String.length() * sizeof(char));
	strcpy(memcached_value, param1String.c_str());

	double ttl = info[2]->NumberValue();

	Callback* callback = new Callback(info[3].As<v8::Function>());

	Job* job = new SetJob(memClient, callback, memcached_key, memcached_value, (int) ttl);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Get(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	v8::String::Utf8Value param0(info[0]->ToString());
	std::string param0String = std::string(*param0);
	char* memcached_key = (char*) malloc(param0String.length() * sizeof(char));
	strcpy(memcached_key, param0String.c_str());

	Callback* callback = new Callback(info[1].As<v8::Function>());

	Job* job = new GetJob(memClient, callback, memcached_key);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Touch(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	v8::String::Utf8Value param0(info[0]->ToString());
	std::string param0String = std::string(*param0);
	char* memcached_key = (char*) malloc(param0String.length() * sizeof(char));
	strcpy(memcached_key, param0String.c_str());

	double ttl = info[1]->NumberValue();

	Callback* callback = new Callback(info[2].As<v8::Function>());

	Job* job = new TouchJob(memClient, callback, memcached_key, (int) ttl);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Stop(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	memClient->backgroundThread->setCallback(new Callback(info[0].As<v8::Function>()));
	memClient->isRunning = false;
}
