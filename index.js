'use strict';

var addon = require('./build/Release/memcachedNative');

var Client = addon.Client;

var c = new Client();

c.start(console.log);

c.set('foo', 33, 10, console.log);

setTimeout(function() {
  console.log('aaaaa');
}, 100);
