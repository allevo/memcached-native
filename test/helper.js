'use strict';

var crypto = require('crypto');
var net = require('net');
var async = require('async');

function getRandomString(length) {
  length = length || 5;
  var alphabeth = 'qwertyuiopasdfghjklzxcvbnm'.split('');
  var s = '';
  for(var i = 0; i < length; i++) {
    s += alphabeth[crypto.pseudoRandomBytes(1)[0] % alphabeth.length];
  }
  return s;
}

function getAllItems(callback) {
  var PORT = getMemcachePort();
  var HOST = getMemcacheHost();

  var client = new net.Socket();
  client.connect(PORT, HOST, function() {
    client.write('stats cachedump 1 100\n');
  });

  client.on('data', function(buf) {
    client.destroy();

    var lines = buf.toString().split('\r\n');
    // remove end
    lines.pop();
    lines.pop();

    async.map(lines, function(l, next) {
      var matches = l.match(/^ITEM (\w+) \[\d+ b; (\d+) s\]/);
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

function getMemcachePort() {
  return parseInt(process.env.MEMCACHE_PORT_11211_TCP_PORT || '11211', 10);
}

function getMemcacheHost() {
  return process.env.MEMCACHE_PORT_11211_TCP_ADDR || '127.0.0.1';
}


module.exports = {
  getRandomString: getRandomString,
  getAllItems: getAllItems,
  getMemcachePort: getMemcachePort,
  getMemcacheHost: getMemcacheHost,
};
