test_reader = executable('test_reader',
  sources: 'test_reader.cpp',
  include_directories: tyche_include_dir,
  dependencies: [gtest_dep],
)
test('test_reader', test_reader)

test_toml_reader = executable('test_toml_reader',
  sources: 'test_toml_reader.cpp',
  include_directories: tyche_include_dir,
  dependencies: [gtest_dep, tomlplusplus_dep],
)
test('test_toml_reader', test_toml_reader)

