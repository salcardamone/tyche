test_atom_type_reader = executable('test_atom_type_reader',
  sources: 'test_atom_type_reader.cpp',
  include_directories: tyche_include_dir,
  link_with: atom_lib,
  dependencies: [gtest_dep, tomlplusplus_dep],
)
test('test_atom_type_reader', test_atom_type_reader)

test_atomic_state_reader = executable('test_atomic_state_reader',
  sources: 'test_atomic_state_reader.cpp',
  include_directories: tyche_include_dir,
  link_with: atom_lib,
  dependencies: [gtest_dep, tomlplusplus_dep],
)
test('test_atomic_state_reader', test_atomic_state_reader)
