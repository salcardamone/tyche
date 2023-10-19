integrate_lib_sources = ['velocity_verlet.cpp', 'velocity_verlet_nvt_evans.cpp']
integrate_lib = shared_library('integrate',
  integrate_lib_sources,
  include_directories: tyche_include_dir,
  link_with: [atom_lib, system_lib, force_lib],
  dependencies: [spdlog_dep, tomlplusplus_dep]
)
