{
  "targets": [
    {
      "target_name": "memcachedNative",
      "sources": [
        "src/init.cc",
        "src/utils.cpp",
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
