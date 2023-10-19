system_lib_sources = ['cell.cpp', 'thermostat.cpp']
system_lib = shared_library('system',
  system_lib_sources,
  include_directories: tyche_include_dir,
  link_with: atom_lib,
  dependencies: [spdlog_dep]
)
