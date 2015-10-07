'use strict';

var assert = require('assert');
var index = require('../index');

describe('other', function () {
  describe('constants', function () {
    it('should be exported', function () {
      assert.equal(0, index.MEMCACHED_SUCCESS);
      assert.equal(1, index.MEMCACHED_FAILURE);
    });
  });
  describe('strerror', function () {
    it('should be exported', function () {
      var str = index.strerror(index.MEMCACHED_SUCCESS);
      assert.equal('SUCCESS', str);
      var str = index.strerror(index.MEMCACHED_FAILURE);
      assert.equal('FAILURE', str);
    });
  });
  describe('isSuccess', function () {
    it('should be exported', function () {
      assert.equal(true, index.isSuccess(index.MEMCACHED_SUCCESS));
      assert.equal(false, index.isSuccess(index.MEMCACHED_FAILURE));
    });
  });
  describe('isFailed', function () {
    it('should be exported', function () {
      assert.equal(false, index.isFailed(index.MEMCACHED_SUCCESS));
      assert.equal(true, index.isFailed(index.MEMCACHED_FAILURE));
    });
  });
  describe('isFatal', function () {
    it('should be exported', function () {
      assert.equal(false, index.isFatal(index.MEMCACHED_SUCCESS));
      assert.equal(true, index.isFatal(index.MEMCACHED_FAIL_UNIX_SOCKET));
    });
  });
});
