'use strict';

var assert = require('assert');
var spawn = require('child_process').spawn;
var net = require('net');
var async = require('async');
var crypto = require('crypto');

var Client = require('../index').Client;

function getRandomString() {
  var alphabeth = 'qwertyuiopasdfghjklzxcvbnm'.split('');
  var s = '';
  for(var i = 0; i < 5; i++) {
    s += alphabeth[crypto.pseudoRandomBytes(1)[0] % alphabeth.length];
  }
  return s;
}

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
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        getAllItems(function(err, items) {
          assert.ifError(err);

          var diff = items[key].s - Math.round(Date.now() / 1000);
          assert.ok(diff < 11, 'Diff too high ' + diff);
          assert.ok(diff > 5, 'Diff too low ' + diff);
          assert.equal(items[key].value, value);

          done();
        });
      });
    });

    it('get should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        setTimeout(function() {
          memcachedClient.get(key, function(err, data) {
            assert.ifError(err);
            assert.equal(data, value);
            done();
          });
        }, 10);
      });
    });

    it('touch should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        setTimeout(function() {
          memcachedClient.touch(key, 200, function(err) {

            getAllItems(function(err, items) {
              assert.ifError(err);

              var diff = items[key].s - Math.round(Date.now() / 1000);
              assert.ok(diff < 201, 'Diff too high ' + diff);
              assert.ok(diff > 195, 'Diff too low ' + diff);
              done();
            });
          });
        }, 10);
      });
    });

    it('increment should works fine', function(done) {
      var key = getRandomString();
      memcachedClient.set(key, 23, 10, function(err) {
        assert.ifError(err);

        setTimeout(function() {
          memcachedClient.increment(key, 40, function(err, finalValue) {
            assert.ifError(err);
            assert.equal(23 + 40, finalValue);

            memcachedClient.get(key, function(err, res) {
              assert.ifError(err)
              assert.equal(23 + 40, res);

              done();
            });
          });
        }, 10);
      });
    });

    it('decrement should works fine', function(done) {
      var key = getRandomString();
      memcachedClient.set(key, 23, 10, function(err) {
        assert.ifError(err);

        setTimeout(function() {
          memcachedClient.decrement(key, 3, function(err, finalValue) {
            assert.ifError(err);
            assert.equal(23 - 3, finalValue);

            memcachedClient.get(key, function(err, res) {
              assert.ifError(err)
              assert.equal(23 - 3, res);

              done();
            });
          });
        }, 10);
      });
    });

    it('append should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        memcachedClient.append(key, '-suffix', function(err) {
          console.log(err);
          assert.ifError(err);

          memcachedClient.get(key, function(err, res) {
            assert.ifError(err)

            assert.equal(value + '-suffix', res);

            done();
          });
        });
      });
    });

    it('prepend should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        memcachedClient.prepend(key, 'prefix-', function(err) {
          assert.ifError(err);

          memcachedClient.get(key, function(err, res) {
            assert.ifError(err)

            assert.equal('prefix-' + value, res);

            done();
          });
        });
      });
    });

    it('delete should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        memcachedClient.delete(key, 0, function(err) {
          assert.ifError(err);

          memcachedClient.get(key, function(err, res) {
            assert.equal(err.message, 'NOT FOUND');
            assert.equal(err.code, 'NOT FOUND');
            assert.equal(err.errno, require('../index').MEMCACHED_NOTFOUND);

            done();
          });
        });
      });
    });

    it('exist should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        memcachedClient.exist(key, function(err, isExist) {
          assert.ifError(err);

          assert.equal(true, isExist);

          memcachedClient.exist(key + '-foo', function(err, isExist) {
            assert.ifError(err);

            assert.equal(false, isExist);
            done();
          });
        });
      });
    });

    it('replace should works fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        var value2 = getRandomString();
        memcachedClient.replace(key, value2, 10, function(err) {
          assert.ifError(err);

          memcachedClient.get(key, function(err, val) {
            assert.ifError(err);

            assert.equal(value2, val);
            done();
          });
        });
      });
    });
  });
});
