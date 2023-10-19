simulation_lib_sources = ['molecular_dynamics.cpp']
simulation_lib = shared_library('simulation',
  simulation_lib_sources,
  include_directories: tyche_include_dir,
  link_with: [atom_lib, system_lib, force_lib, integrate_lib],
  dependencies: [spdlog_dep, tomlplusplus_dep]
)
