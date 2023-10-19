test_velocity_verlet = executable('test_velocity_verlet',
  sources: 'test_velocity_verlet.cpp',
  include_directories: tyche_include_dir,
  link_with: [atom_lib, system_lib, force_lib, integrate_lib],
  dependencies: [gtest_dep, tomlplusplus_dep],
)
test('test_velocity_verlet', test_velocity_verlet)
