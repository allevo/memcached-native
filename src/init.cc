
#include <nan.h>

#include "client.hpp"

/*
memcached_st* client = NULL;

bool test(memcached_st* client) {
    memcached_return_t rc;
	const char *key= "foo";
	const char *value= "value";
	rc = memcached_set(client, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);
	if (!memcached_success(rc)) {
		if (memcached_fatal(rc)) { // drop connection
			printf("%s %s\n", "FATAL", memcached_strerror(NULL, rc));
		} else if (memcached_failed(rc)) { // callback
			printf("%s %s\n", "FAILED", memcached_strerror(NULL, rc));
		}
		return 0;
	}

    uint32_t flags= 0;
    size_t value_length= 0;
	char* ss = memcached_get(client, key, strlen(key), &value_length, &flags, &rc);

	if (!memcached_success(rc)) {
		if (memcached_fatal(rc)) { // drop connection
			printf("%s %s\n", "FATAL", memcached_strerror(NULL, rc));
		} else if (memcached_failed(rc)) { // callback
			printf("%s %s\n", "FAILED", memcached_strerror(NULL, rc));
		}
		return 0;
	}
	printf("%s %s %d %s\n", "RET", ss, value_length, memcached_strerror(NULL, rc));

	return 1;
}


const char* config_string= "--SERVER=localhost --SERVER=localhost:11212";
void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {

    test(client);

	// memcached_free(client);
	info.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}
*/
void Init(v8::Local<v8::Object> exports) {
	// client = memcached(config_string, strlen(config_string));

	MemcachedNative::Client::Init(exports);
	/*
	exports->Set(
		Nan::New("hello").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(Method)->GetFunction()
	);
	*/
}

NODE_MODULE(memcachedNative, Init)
