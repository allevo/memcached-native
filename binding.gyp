{
  "targets": [
    {
      "target_name": "memcachedNative",
      "sources": [
        "src/init.cc",
        "src/client.cpp"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      'link_settings': {
          'libraries': [
              '<!@(pkg-config --libs libmemcached)'
          ]
      }
    }
  ]
}
