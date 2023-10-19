force_lib_sources = ['lennard_jones.cpp']
force_lib = shared_library('force',
  force_lib_sources,
  include_directories: tyche_include_dir,
  link_with: atom_lib,
  dependencies: [spdlog_dep, tomlplusplus_dep]
)
