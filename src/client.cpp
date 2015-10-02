#include "client.hpp"

#include <unistd.h>

using namespace MemcachedNative;
using Nan::AsyncProgressWorker;
using Nan::Callback;

Nan::Persistent<v8::Function> Client::constructor;



class Job {
public:
	Job(MemcachedNative::Client* client_)
		: client(client_) { }

	virtual void execute() = 0;

private:
	MemcachedNative::Client* client;
};

class SetJob : public Job {
public:
	SetJob(MemcachedNative::Client* client_)
		: Job(client_) { }

	virtual void execute() {
		printf("%s\n", "SetJob execute");
	}
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

		while(true) {
			usleep(1000 * 1000);
			printf("%s\n", "whiling...");
			for(std::set<Job*>::iterator ii = client->jobs.begin(); ii != client->jobs.end(); ii++) {
				(*ii)->execute();
			}
		}

	}

	virtual void HandleProgressCallback(const char *data, size_t size) {
		printf("%s\n", "HandleProgressCallback MemcachedAsyncProgressWorker");
	}

private:
	Client* client;
};


Client::Client(const char* config_string) {
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
	Nan::AsyncQueueWorker(new MemcachedAsyncProgressWorker(memClient, callback));

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Set(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	Job* job = new SetJob(memClient);

	memClient->jobs.insert(job);

	// Callback* callback = new Callback(info[0].As<v8::Function>());
	// MemcachedAsyncProgressWorker* worker = new MemcachedAsyncProgressWorker(memClient, callback);

	info.GetReturnValue().Set(Nan::Undefined());
}
