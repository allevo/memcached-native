# memcached-native
[![Build Status](https://travis-ci.org/allevo/memcached-native.svg?branch=master)](https://travis-ci.org/allevo/memcached-native)
This package wraps the library for memcache written in C.

## Client
This class is used to connect to memcache server. All expirationTime is misured in seconds. Please see (http://stackoverflow.com/questions/1418324/memcache-maximum-key-expiration-time)

### Constructor
Use the memcached configuration language to setup the client correctly:

``` var memcachedClient = new Client('--SERVER=127.0.0.1:11211'); ```

No fallback is made

### Start
You should always call this method before doing anything

``` memcachedClient.start(); ```

### Stop
After all, this library needed to be stopped

``` memcachedClient.stop(done); ```

### Set
``` memcachedClient.set(key, value, expirationTime, callback); ```

Set a new value to the server. The third parameter is the expiration time in sec. The callback takes a parameter, the error.
```
memcachedClient.set(key, value, 10, function(err) {
  assert.ifError(err);
  console.log('Set');
});
```

### Get
``` memcachedClient.get(key, callback); ```

Retrieve the value saved as `key`
```
memcachedClient.get(key, function(err, data) {
  assert.ifError(err);
  console.log(data);
  done();
});
```

### Touch
``` memcachedClient.touch(key, expirationTime, callback); ```

Touch a key
```
memcachedClient.touch(key, 200, function(err) {
  assert.ifError(err);
  console.log('Touched');
});

```

### Increment
``` memcachedClient.increment(key, delta, callback); ```

Incremnt key value by delta.
```
memcachedClient.increment(key, 40, function(err, finalValue) {
  assert.ifError(err);
  console.log('Incremented. Final value is', finalValue);
});
```

### Decrement
``` memcachedClient.decrement(key, 3, callback); ```

Decrement key value by delta. No negative value is stored on the server (forced to be 0 at least)
```
memcachedClient.decrement(key, 3, function(err, finalValue) {
  assert.ifError(err);
  console.log('Decremented. Final value is', finalValue);
});
```

### Append
``` memcachedClient.append(key, '-suffix', callback); ```

Append a string before a stored value.
```
memcachedClient.append(key, '-suffix', function(err) {
  assert.ifError(err);
});
```

### Prepend
``` memcachedClient.append(key, '-suffix', callback); ```

Prepend a string before a stored value.
```
memcachedClient.prepend(key, 'prefix-', function(err) {
  assert.ifError(err);
});
```

### Delete
``` memcachedClient.delete(key, expirationTime); ```

Delete a key from the server. ExpirationTime is often 0 (now).
```
memcachedClient.delete(key, 0, function(err) {
  assert.ifError(err);
});
```

### Exist
``` memcachedClient.exist(key, callback); ```

Check if a key is set to memcached without retrieving it.
```
memcachedClient.exist(key, function(err, isExist) {
  assert.ifError(err);
  console.log(isExist ? 'exists' : 'no');
});
```

### Replace
``` memcachedClient.replace(key, value, expirationTime, callback); ```

Replace a value already presents on the server.
```
memcachedClient.replace(key, value, 10, function(err) {
  assert.ifError(err);
});
```

### Cas
``` memcachedClient.cas(key, value, expirationTime, casValue, callback); ```

Cas operation. `casValue` can be retrieved from `fetch_result` or `mget_and_fetch_all` methods.
```
memcachedClient.cas(key, value, 10, casValue, function(err) {
  assert.ifError(err);
});
```

### MGet
``` memcachedClient.mget(keys, callback); ```

Fetch an array of keys (see fetch_result)
```
memcachedClient.mget(['key1', 'key2'], function(err) {
  assert.ifError(err);
});
```

### FetchResult
``` memcachedClient.fetch_result(callback); ```

Return the key fetched by mget method. Be careful.
```
memcachedClient.mget(['key1', 'key2'], function(err) {
  memcachedClient.fetch_result(function(err, results) {
    assert.ifError(err);
    console.log(results.key1);
    console.log(results.key2);
  });
});
```

### MGetAndFetchAll
``` memcachedClient.mget_and_fetch_all(keys, callback); ```

Request and return all keys specified
```
memcachedClient.mget_and_fetch_all(['key1', 'key2'], function(err, results) {
  assert.ifError(err);
  console.log(results.key1);
  console.log(results.key2);
});
```

## Some considerations
When you run a client, a thread is started. Nodejs uses 4 thread to make some operations (like read file) that cannot be performed as asynchronous operation.

All suggestions are welcome
