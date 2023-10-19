test_molecular_dynamics = executable('test_molecular_dynamics',
  sources: 'test_molecular_dynamics.cpp',
  include_directories: tyche_include_dir,
  link_with: [atom_lib, system_lib, force_lib, integrate_lib, simulation_lib],
  dependencies: [gtest_dep, tomlplusplus_dep],
)
test('test_molecular_dynamics', test_molecular_dynamics)
