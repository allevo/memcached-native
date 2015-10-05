'use strict';

var addon = require('./build/Release/memcachedNative');
module.exports = addon.Client;
/*
var Client = addon.Client;

var c = new Client();

 c.start();
/*
var i = 0;
setInterval(function() {
  for(var j=0; j < 30; j++) {
    i++;
    c.set('foo', 33, 10, console.log.bind(console, 'SET IS DONE', i));
  }
}, 10);
*/
/*
// setInterval(function() {
  c.set('foo', 5688, 100, function() {
    // console.log('SET ID DONE');
    c.get('foo', function(err, data) {
      console.log('GET', err, data);
    });
  });
// });

// c.touch(33, 33, console.log.bind(console, 'AA'), console.log.bind(console, 'BB'));
/*
setTimeout(function() {
  c.stop(function() {
    console.log('end');
  });
}, 1000);
*/