'use strict';

var assert = require('assert');
var Client = require('../index').Client;

var helper = require('./helper');
var getRandomString = helper.getRandomString;

var PORT = helper.getMemcachePort();
var HOST = helper.getMemcacheHost();

var memcachedClient;
xdescribe('benchmark', function () {
  beforeEach(function() {
    memcachedClient = new Client('--SERVER=' + HOST + ':' + PORT + ' --SUPPORT-CAS');
    memcachedClient.start();
  });
  afterEach(function(done) {
    memcachedClient.stop(done);
  });
  describe('serie', function () {
    function getSetSerieWrapper(n, fn, done) {
      var i = 0;
      return function set() {
        i++;
        if (i >= n) return done();
        fn(function(err) {
          assert.ifError(err);
          set();
        });
      }
    }

    it('client set small value should be efficient', function (done) {
      var key = getRandomString();
      var value = 'value';
      getSetSerieWrapper(1000, memcachedClient.set.bind(memcachedClient, key, value, 42), done)();
    });
    it('client set largest value should be efficient', function (done) {
      var key = getRandomString();
      var value = (new Array(100001)).join('a');
      getSetSerieWrapper(1000, memcachedClient.set.bind(memcachedClient, key, value, 42), done)();
    });
    it('client set heavy value should be efficient', function (done) {
      var key = getRandomString();
      // Memcached max stores 1M (by default)
      var value = (new Array(1000001)).join('a');
      getSetSerieWrapper(10, memcachedClient.set.bind(memcachedClient, key, value, 42), done)();
    });
  });

  describe('parallel', function () {
    function setParallel(n, fn, done) {
      var i = 0;
      function cbk(err) {
        assert.ifError(err);
        i++;
        if (i === n) {
          return done();
        }
      }
      for(var j = 0; j < n; j++) {
        fn(cbk);
      }
    }

    it('client set small value should be efficient', function (done) {
      var key = getRandomString();
      var value = 'value';
      setParallel(1000, memcachedClient.set.bind(memcachedClient, key, value, 42), done);
    });
    it('client set largest value should be efficient', function (done) {
      var key = getRandomString();
      var value = (new Array(100001)).join('a');
      setParallel(1000, memcachedClient.set.bind(memcachedClient, key, value, 42), done);
    });
    it('client set heavy value should be efficient', function (done) {
      var key = getRandomString();
      // Memcached max stores 1M (by default)
      var value = (new Array(1000001)).join('a');
      setParallel(10, memcachedClient.set.bind(memcachedClient, key, value, 42), done);
    });
  });
});
