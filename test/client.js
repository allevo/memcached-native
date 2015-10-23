'use strict';

var assert = require('assert');
var spawn = require('child_process').spawn;
var net = require('net');

var Client = require('../index').Client;

var helper = require('./helper');
var getRandomString = helper.getRandomString;
var getAllItems = helper.getAllItems;

var PORT = helper.getMemcachePort();
var HOST = helper.getMemcacheHost();

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
      memcachedClient = new Client('--SERVER=' + HOST + ':' + PORT + ' --SUPPORT-CAS');
      memcachedClient.debug(true);
      memcachedClient.start();
    });
    afterEach(function stopMemcachedClient(done) {
      memcachedClient.stop(done);
    });

    it('set should work fine', function(done) {
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

    it('get should work fine', function(done) {
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

    it('touch should work fine', function(done) {
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

    it('increment should work fine', function(done) {
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

    it('decrement should work fine', function(done) {
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

    it('append should work fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();
      memcachedClient.set(key, value, 10, function(err) {
        assert.ifError(err);

        memcachedClient.append(key, '-suffix', function(err) {
          assert.ifError(err);

          memcachedClient.get(key, function(err, res) {
            assert.ifError(err)

            assert.equal(value + '-suffix', res);

            done();
          });
        });
      });
    });

    it('prepend should work fine', function(done) {
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

    it('delete should work fine', function(done) {
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

    it('exist should work fine', function(done) {
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

    it('replace should work fine', function(done) {
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

    it('cas should work fine', function(done) {
      var key = getRandomString();
      var value = getRandomString();

      memcachedClient.set(key, value, 10, function(err) {
      assert.ifError(err);
      memcachedClient.mget_and_fetch_all([key], function(err, results) {
      assert.ifError(err);

        var newValue = getRandomString();
        memcachedClient.cas(key, newValue, 30, results[key].cas, function(err) {
          assert.ifError(err);

          memcachedClient.get(key, function(err, value) {
            assert.ifError(err);

            assert.equal(value, newValue);

            done();
          });
        });
      });
      });
    });

    it('mget and fetch_result should work fine', function(done) {
      var keys = [
        getRandomString(),
        getRandomString(),
        getRandomString(),
      ];
      var values = [
        getRandomString(),
        getRandomString(),
        getRandomString(),
      ];
      memcachedClient.set(keys[0], values[0], 10, function(err) {
      assert.ifError(err);
      memcachedClient.set(keys[1], values[1], 10, function(err) {
      assert.ifError(err);
      memcachedClient.set(keys[2], values[2], 10, function(err) {
      assert.ifError(err);

        memcachedClient.mget(keys, function(err) {
          assert.ifError(err);

          memcachedClient.fetch_result(function(err, results) {
            assert.ifError(err);

            assert.deepEqual(Object.keys(results), keys);
            assert.equal(results[keys[0]].value, values[0]);
            assert.equal(results[keys[1]].value, values[1]);
            assert.equal(results[keys[2]].value, values[2]);
            assert.ok(results[keys[0]].cas);
            assert.ok(results[keys[1]].cas);
            assert.ok(results[keys[2]].cas);

            done();
          });
        });
      });
      });
      });
    });

    it('mget_and_fetch_all should work fine', function (done) {
      var keys = [
        getRandomString(),
        getRandomString(),
        getRandomString(),
      ];
      var values = [
        getRandomString(),
        getRandomString(),
        getRandomString(),
      ];
      memcachedClient.set(keys[0], values[0], 10, function(err) {
      assert.ifError(err);
      memcachedClient.set(keys[1], values[1], 10, function(err) {
      assert.ifError(err);
      memcachedClient.set(keys[2], values[2], 10, function(err) {
      assert.ifError(err);

        memcachedClient.mget_and_fetch_all(keys, function(err, results) {
          assert.ifError(err);

          assert.deepEqual(Object.keys(results), keys);
          assert.equal(results[keys[0]].value, values[0]);
          assert.equal(results[keys[1]].value, values[1]);
          assert.equal(results[keys[2]].value, values[2]);
          assert.ok(results[keys[0]].cas);
          assert.ok(results[keys[1]].cas);
          assert.ok(results[keys[2]].cas);

          done();
        });
      });
      });
      });
    });
  });
});
