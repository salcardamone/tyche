test_cell = executable('test_cell',
  sources: 'test_cell.cpp',
  include_directories: tyche_include_dir,
  link_with: system_lib,
  dependencies: [gtest_dep, tyche_dep],
)
test('test_cell', test_cell)

test_thermostat = executable('test_thermostat',
  sources: 'test_thermostat.cpp',
  include_directories: tyche_include_dir,
  link_with: [atom_lib, system_lib],
  dependencies: [gtest_dep, tyche_dep, tomlplusplus_dep],
)
test('test_thermostat', test_thermostat)

