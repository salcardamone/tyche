subdir('atom')
subdir('force')

tyche_lib = shared_library('tyche',
  sources: ['simulation/molecular_dynamics.cpp'],
  include_directories: tyche_include_dir,
  link_with: [atom_lib],
  dependencies: [spdlog_dep, tomlplusplus_dep]
)

tyche_dep = declare_dependency(
  link_with: tyche_lib,
  include_directories: tyche_include_dir
)

