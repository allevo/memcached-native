{
  "targets": [
    {
      "target_name": "memcachedNative",
      "sources": [ "src/init.cc", "src/client.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "cflags": [
        "-std=c++11",
        "-stdlib=libc++"
      ],
      'link_settings': {
          'libraries': [
              '-lmemcached'
          ]
      }
    }
  ]
}
