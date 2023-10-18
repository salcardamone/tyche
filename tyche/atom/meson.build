atom_lib_sources = ['atom_type.cpp']
atom_lib = shared_library('atom',
  atom_lib_sources,
  include_directories: tyche_include_dir,
  dependencies: [spdlog_dep, tomlplusplus_dep]
)
