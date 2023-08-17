tyche_lib = shared_library('tyche',
  sources: ['atom_type.cpp'],
  include_directories: tyche_include_dir,
  dependencies: [spdlog_dep]
)

tyche_dep = declare_dependency(
  link_with: tyche_lib,
  include_directories: tyche_include_dir
)

