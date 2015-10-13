#include "client.hpp"

#include "MemcachedAsyncProgressWorker.hpp"
#include "Job/SetJob.hpp"
#include "Job/GetJob.hpp"
#include "Job/TouchJob.hpp"
#include "Job/IncrementJob.hpp"
#include "Job/DecrementJob.hpp"

using namespace MemcachedNative;

using v8::Local;
using v8::Value;
using v8::Exception;
using v8::Object;

using Nan::AsyncProgressWorker;
using Nan::Callback;


Nan::Persistent<v8::Function> Client::constructor;

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
	Nan::SetPrototypeMethod(tpl, "increment", Increment);
	Nan::SetPrototypeMethod(tpl, "decrement", Decrement);

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

	JobBase* job = new SetJob(memClient, callback, memcached_key, memcached_value, (int) ttl);
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

	JobBase* job = new GetJob(memClient, callback, memcached_key);
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

	JobBase* job = new TouchJob(memClient, callback, memcached_key, (int) ttl);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Increment(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	v8::String::Utf8Value param0(info[0]->ToString());
	std::string param0String = std::string(*param0);
	char* memcached_key = (char*) malloc(param0String.length() * sizeof(char));
	strcpy(memcached_key, param0String.c_str());

	uint32_t delta = info[1]->NumberValue();

	Callback* callback = new Callback(info[2].As<v8::Function>());

	JobBase* job = new IncrementJob(memClient, callback, memcached_key, delta);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Decrement(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	v8::String::Utf8Value param0(info[0]->ToString());
	std::string param0String = std::string(*param0);
	char* memcached_key = (char*) malloc(param0String.length() * sizeof(char));
	strcpy(memcached_key, param0String.c_str());

	uint32_t delta = info[1]->NumberValue();

	Callback* callback = new Callback(info[2].As<v8::Function>());

	JobBase* job = new DecrementJob(memClient, callback, memcached_key, delta);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Stop(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	memClient->backgroundThread->setCallback(new Callback(info[0].As<v8::Function>()));
	memClient->isRunning = false;
}
