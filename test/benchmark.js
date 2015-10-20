'use strict';

var assert = require('assert');
var Client = require('../index').Client;
var async = require('async');

var helper = require('./helper');
var getRandomString = helper.getRandomString;

var PORT = helper.getMemcachePort();
var HOST = helper.getMemcacheHost();

var memcachedClient;
describe('benchmark', function () {
  beforeEach(function() {
    memcachedClient = new Client('--SERVER=' + HOST + ':' + PORT + ' --SUPPORT-CAS');
    memcachedClient.start();
  });
  afterEach(function(done) {
    memcachedClient.stop(done);
  });
  it('client set small value should be efficient', function (done) {
    var key = getRandomString();
    var value = 'value';
    var ops = [];
    for(var i = 0; i < 1000; i++) {
      ops.push(memcachedClient.set.bind(memcachedClient, key, value, 42));
    }
    async.series(ops, done);
  });
  it('client set largest value should be efficient', function (done) {
    var key = getRandomString();
    var value = (new Array(100001)).join('a');
    var ops = [];
    for(var i = 0; i < 1000; i++) {
      ops.push(memcachedClient.set.bind(memcachedClient, key, value, 42));
    }
    async.series(ops, done);
  });
  it('client set largest value should be efficient', function (done) {
    var key = getRandomString();
    // Memcached max stores 1M (by default)
    var value = (new Array(1000001)).join('a');
    var ops = [];
    for(var i =0; i < 10; i++) {
      ops.push(memcachedClient.set.bind(memcachedClient, key, value, 42));
    }
    async.series(ops, done);
  });
});
