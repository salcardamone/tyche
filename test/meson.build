gtest_dep = dependency('gtest', main: true, required: true)

test_atom_type_reader = executable('test_atom_type_reader',
  sources: 'test_atom_type_reader.cpp',
  dependencies: [gtest_dep, tyche_dep],
)
test('test_atom_type_reader', test_atom_type_reader)

test_tensor = executable('test_tensor',
  sources: 'test_tensor.cpp',
  dependencies: [gtest_dep, tyche_dep],
)
test('test_tensor', test_tensor)

test_atomic_state_reader = executable('test_atomic_state_reader',
  sources: 'test_atomic_state_reader.cpp',
  dependencies: [gtest_dep, tyche_dep],
)
test('test_atomic_state_reader', test_atomic_state_reader)

test_cell = executable('test_cell',
  sources: 'test_cell.cpp',
  dependencies: [gtest_dep, tyche_dep],
)
test('test_cell', test_cell)

subdir('io')
subdir('system')
subdir('force')
subdir('integrate')
subdir('simulation')
