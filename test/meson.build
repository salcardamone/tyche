gtest_dep = dependency('gtest', main: true, required: true)


test_atom_type_reader = executable('test_atom_type_reader',
  sources: 'test_atom_type_reader.cpp',
  dependencies: [gtest_dep, tyche_dep],
)
test('test_atom_type_reader', test_atom_type_reader)
