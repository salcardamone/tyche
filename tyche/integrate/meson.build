integrate_lib_sources = [
  'integrate_factory.cpp',
  'velocity_verlet.cpp',
  'velocity_verlet_nvt_evans.cpp',
  'velocity_verlet_nvt_andersen.cpp',
]

integrate_lib = shared_library('integrate',
  integrate_lib_sources,
  include_directories: tyche_include_dir,
  link_with: [atom_lib, system_lib, force_lib],
  dependencies: [spdlog_dep]
)
