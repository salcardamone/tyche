subdir('atom')
subdir('system')
subdir('force')
subdir('integrate')
subdir('simulation')

tyche_dep = declare_dependency(
  link_with: [atom_lib, system_lib, force_lib, integrate_lib, simulation_lib],
  include_directories: tyche_include_dir
)

