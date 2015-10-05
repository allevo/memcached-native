'use strict';

var assert = require('assert');
var spawn = require('child_process').spawn;
var net = require('net');
var async = require('async');

var Client = require('../index');

var PORT = 11212;
function getAllItems(callback) {
  var client = new net.Socket();
  client.connect(PORT, '127.0.0.1', function() {
    client.write('stats cachedump 1 100\n');
  });

  client.on('data', function(buf) {

    client.destroy();

    console.log('DATA: ' + buf);
    var lines = buf.toString().split('\n');
    // remove end
    lines.pop();
    // remove end
    lines.pop();

    async.map(lines, function(l, next) {
      console.log('L', l);
      var matches = l.match(/^ITEM (\w+) \[\d b; (\d+) s\]/);
      var item = {
        key: matches[1],
        s: matches[2],
      };
      var client2 = new net.Socket();
      client2.connect(PORT, '127.0.0.1', function() {
        console.log(item);
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
    beforeEach(function(done) {
      memcachedServerProcess = spawn('memcached', ['-p', PORT + '', '-vvv']);
      setTimeout(done, 100);
    });
    afterEach(function() {
      memcachedServerProcess.kill('SIGTERM');
    });
    beforeEach(function() {
      memcachedClient = new Client('--SERVER=localhost:' + PORT);
      memcachedClient.start();
    })
    afterEach(function(done) {
      memcachedClient.stop(done);
    })
    it('set should works fine', function(done) {
      memcachedClient.set('foo', 'value', 10, function(err) {
        assert.ifError(err);

        getAllItems(function(err, items) {
          assert.ifError(err);

          var diff = items.foo.s - Math.round(Date.now() / 1000);
          console.log(diff);
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