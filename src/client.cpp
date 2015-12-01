#include "client.hpp"

#include <ctime>

#include "utils.hpp"

#include "Job/SetJob.hpp"
#include "Job/GetJob.hpp"
#include "Job/TouchJob.hpp"
#include "Job/IncrementJob.hpp"
#include "Job/DecrementJob.hpp"
#include "Job/PrependJob.hpp"
#include "Job/AppendJob.hpp"
#include "Job/DeleteJob.hpp"
#include "Job/ExistJob.hpp"
#include "Job/ReplaceJob.hpp"
#include "Job/CasJob.hpp"
#include "Job/MGetJob.hpp"
#include "Job/FetchResultJob.hpp"
#include "Job/MGetAndFetchAllJob.hpp"

#include "MemcachedAsyncProgressWorker.hpp"

using namespace MemcachedNative;

using v8::Local;
using v8::Value;
using v8::Exception;
using v8::Object;

using Nan::AsyncProgressWorker;
using Nan::Callback;

Nan::Persistent<v8::Function> Client::constructor;

Client::Client(const char* config_string)
	: debug(false), isRunning(false)
{
	debug && printf("%s %s (%lu)\n", "client constructor", config_string, strlen(config_string));
	this->pool = memcached_pool(config_string, strlen(config_string));
}

Client::~Client() {
	memcached_pool_destroy(pool);
}

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
	Nan::SetPrototypeMethod(tpl, "touch", Touch);
	Nan::SetPrototypeMethod(tpl, "increment", Increment);
	Nan::SetPrototypeMethod(tpl, "decrement", Decrement);
	Nan::SetPrototypeMethod(tpl, "append", Append);
	Nan::SetPrototypeMethod(tpl, "prepend", Prepend);
	Nan::SetPrototypeMethod(tpl, "delete", Delete);
	Nan::SetPrototypeMethod(tpl, "exist", Exist);
	Nan::SetPrototypeMethod(tpl, "replace", Replace);
	Nan::SetPrototypeMethod(tpl, "cas", Cas);
	Nan::SetPrototypeMethod(tpl, "mget", MGet);
	Nan::SetPrototypeMethod(tpl, "fetch_result", FetchResult);
	Nan::SetPrototypeMethod(tpl, "mget_and_fetch_all", MGetAndFetchAll);
	Nan::SetPrototypeMethod(tpl, "debug", Debug);
	Nan::SetPrototypeMethod(tpl, "stop", Stop);

	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Client").ToLocalChecked(), tpl->GetFunction());
}

void Client::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	if (info.IsConstructCall()) {
		v8::String::Utf8Value param0(info[0]->ToString());
		std::string* param0String = new std::string(*param0);
		const char* conf = param0String->c_str();

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
	memClient->isRunning = true;
	memClient->progressWorker = new MemcachedAsyncProgressWorker(memClient, new Callback());
	Nan::AsyncQueueWorker(memClient->progressWorker);
}


void Client::Stop(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 0, "0")
	Callback* callback = new Callback(info[0].As<v8::Function>());

	memClient->progressWorker->setCallback(callback);
	memClient->isRunning = false;
}

void Client::Set(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 4, "4")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_STRING_OR_NUMBER(info, 1, "1")
	CHECK_N_ARGS_IS_A_NUMBER(info, 2, "2");
	CHECK_N_ARGS_IS_A_FUNCTION(info, 3, "3")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	const string memcached_value = GET_STRING_FROM_PARAM(info[1]);
	time_t ttl = (time_t) GET_NUMBER_FROM_PARAM(info[2]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[3]);

	JobBase* job = new SetJob(callback, memcached_key, memcached_value, ttl);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Get(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 2, "2")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 1, "1")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[1]);

	JobBase* job = new GetJob(callback, memcached_key);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Touch(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 3, "3")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_NUMBER(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 2, "2")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	time_t ttl = (time_t) GET_NUMBER_FROM_PARAM(info[1]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[2]);

	JobBase* job = new TouchJob(callback, memcached_key, ttl);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Increment(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 3, "3")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_NUMBER(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 2, "2")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	uint32_t delta = (uint32_t) GET_NUMBER_FROM_PARAM(info[1]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[2]);

	JobBase* job = new IncrementJob(callback, memcached_key, delta);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Decrement(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 3, "3")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_NUMBER(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 2, "2")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	uint32_t delta = (uint32_t) GET_NUMBER_FROM_PARAM(info[1]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[2]);

	JobBase* job = new DecrementJob(callback, memcached_key, delta);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Append(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 3, "3")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_STRING(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 2, "2")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	const string memcached_value = GET_STRING_FROM_PARAM(info[1]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[2]);

	JobBase* job = new AppendJob(callback, memcached_key, memcached_value);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Prepend(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 3, "3")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_STRING(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 2, "2")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	const string memcached_value = GET_STRING_FROM_PARAM(info[1]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[2]);

	JobBase* job = new PrependJob(callback, memcached_key, memcached_value);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Delete(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 3, "3")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_AN_UNSIGNED_INTEGER(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 2, "2")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	uint32_t expirationTime = (uint32_t) GET_NUMBER_FROM_PARAM(info[1]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[2]);

	JobBase* job = new DeleteJob(callback, memcached_key, expirationTime);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Exist(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 2, "2")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 1, "1")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[1]);

	JobBase* job = new ExistJob(callback, memcached_key);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Replace(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 4, "4")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_STRING(info, 1, "1")
	CHECK_N_ARGS_IS_A_NUMBER(info, 2, "2")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 3, "3")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	const string memcached_value = GET_STRING_FROM_PARAM(info[1]);
	time_t ttl = (time_t) GET_NUMBER_FROM_PARAM(info[2]);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[3]);

	JobBase* job = new ReplaceJob(callback, memcached_key, memcached_value, ttl);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Cas(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 5, "5")
	CHECK_N_ARGS_IS_A_STRING(info, 0, "0")
	CHECK_N_ARGS_IS_A_STRING(info, 1, "1")
	CHECK_N_ARGS_IS_A_NUMBER(info, 2, "2")
	CHECK_N_ARGS_IS_A_STRING(info, 3, "3")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 4, "4")

	const string memcached_key = GET_STRING_FROM_PARAM(info[0]);
	const string memcached_value = GET_STRING_FROM_PARAM(info[1]);
	time_t ttl = info[2]->NumberValue();
	// String to uint64_t
	const string casString = GET_STRING_FROM_PARAM(info[3]);
	uint64_t cas;
	sscanf(casString.c_str(), "%" PRIu64, &cas);
	Callback* callback = GET_CALLBACK_FROM_PARAM(info[4]);

	JobBase* job = new CasJob(callback, memcached_key, memcached_value, ttl, cas);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::MGet(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 2, "2")
	CHECK_N_ARGS_IS_AN_ARRAY(info, 0, "0")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 1, "1")

	v8::Local<v8::Object> arr = info[0]->ToObject();
	size_t number_of_keys = arr->GetOwnPropertyNames()->Length();
	memClient->debug && printf("%s %lu %s\n", "Array with", number_of_keys, "keys");

	std::vector<string> keys;
	for(size_t i = 0; i < number_of_keys; i++) {
		keys.push_back(std::string(*(v8::String::Utf8Value(arr->Get(i)->ToString()))));
	}

	Callback* callback = GET_CALLBACK_FROM_PARAM(info[1]);

	JobBase* job = new MGetJob(callback, keys);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::FetchResult(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 1, "1")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 0, "0")

	Callback* callback = GET_CALLBACK_FROM_PARAM(info[0]);

	JobBase* job = new FetchResultJob(callback);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}


void Client::MGetAndFetchAll(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 2, "2")
	CHECK_N_ARGS_IS_AN_ARRAY(info, 0, "0")
	CHECK_N_ARGS_IS_A_FUNCTION(info, 1, "1")

	v8::Local<v8::Object> arr = info[0]->ToObject();
	size_t number_of_keys = arr->GetOwnPropertyNames()->Length();
	memClient->debug && printf("%s %lu %s\n", "Array with", number_of_keys, "keys");

	std::vector<string> keys;
	for(size_t i = 0; i < number_of_keys; i++) {
		keys.push_back(std::string(*(v8::String::Utf8Value(arr->Get(i)->ToString()))));
	}

	Callback* callback = GET_CALLBACK_FROM_PARAM(info[1]);

	JobBase* job = new MGetAndFetchAllJob(callback, keys);
	job->setDebug(memClient->debug);
	memClient->jobs.insert(job);

	info.GetReturnValue().Set(Nan::Undefined());
}

void Client::Debug(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	Client* memClient = ObjectWrap::Unwrap<Client>(info.Holder());

	CHECK_ARGUMENT_LENGTH(info, 1, "1")
	CHECK_N_ARGS_IS_A_BOOLEAN(info, 0, "0")

	memClient->debug = info[0]->ToBoolean()->Value();
	info.GetReturnValue().Set(Nan::Undefined());
}
