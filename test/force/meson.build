test_lennard_jones = executable('test_lennard_jones',
  sources: 'test_lennard_jones.cpp',
  include_directories: tyche_include_dir,
  link_with: [atom_lib, force_lib],
  dependencies: [gtest_dep, tomlplusplus_dep],
)
test('test_lennard_jones', test_lennard_jones)
