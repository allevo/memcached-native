'use strict';

var assert = require('assert');
var spawn = require('child_process').spawn;
var net = require('net');
var async = require('async');

var Client = require('../index').Client;

var PORT = parseInt(process.env.MEMCACHE_PORT_11211_TCP_PORT || '11211', 10);
var HOST = process.env.MEMCACHE_PORT_11211_TCP_ADDR || '127.0.0.1';
function getAllItems(callback) {
  var client = new net.Socket();
  client.connect(PORT, HOST, function() {
    client.write('stats cachedump 1 100\n');
  });

  client.on('data', function(buf) {
    client.destroy();

    var lines = buf.toString().split('\n');
    // remove end
    lines.pop();
    lines.pop();

    async.map(lines, function(l, next) {
      var matches = l.match(/^ITEM (\w+) \[\d b; (\d+) s\]/);
      var item = {
        key: matches[1],
        s: matches[2],
      };
      var client2 = new net.Socket();
      client2.connect(PORT, HOST, function() {
        client2.write('get ' + item.key + '\n');
      });
      client2.on('data', function(buf) {
        var lines = buf.toString().split('\n');
        item.value = lines[1].trim();
        client2.destroy();
        next(null, item);
      });
    }, function(err, results) {
      client.destroy();
      var items = {};
      for(var i in results) {
        items[results[i].key] = results[i];
      }
      callback(err, items);
    });
  });
}

var memcachedServerProcess;
var memcachedClient;
describe('memcached', function () {

  describe('client', function () {
    beforeEach(function startMemcachedServerProcess(done) {
      var client = new net.Socket();
      client.connect(PORT, HOST, function() {
        client.write('flush_all\n');
        client.destroy();
        done();
      });
    });
    beforeEach(function startMemcachedClient() {
      memcachedClient = new Client('--SERVER=' + HOST + ':' + PORT);
      memcachedClient.start();
    });
    afterEach(function stopMemcachedClient(done) {
      memcachedClient.stop(done);
    });
    it('set should works fine', function(done) {
      memcachedClient.set('foo', 'value', 10, function(err) {
        assert.ifError(err);

        getAllItems(function(err, items) {
          assert.ifError(err);

          var diff = items.foo.s - Math.round(Date.now() / 1000);
          assert.ok(diff < 11, 'Diff too high');
          assert.ok(diff > 8, 'Diff too low');
          assert.equal(items.foo.value, 'value');

          done();
        });
      });
    });

    it('get should works fine', function(done) {
      memcachedClient.set('foo', 'value', 10, function(err) {
        assert.ifError(err);

        setTimeout(function() {
          memcachedClient.get('foo', function(err, data) {
            assert.ifError(err);
            assert.equal(data, 'value');
            done();
          });
        }, 10);
      });
    });

    it('touch should works fine', function(done) {
      memcachedClient.set('foo', 'value', 10, function(err) {
        assert.ifError(err);

        setTimeout(function() {
          memcachedClient.touch('foo', 200, function(err) {

            getAllItems(function(err, items) {
              assert.ifError(err);

              var diff = items.foo.s - Math.round(Date.now() / 1000);
              assert.ok(diff < 201, 'Diff too high');
              assert.ok(diff > 198, 'Diff too low');
              done();
            });
          });
        }, 10);
      });
    });
  });
});
