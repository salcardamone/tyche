project('tyche', 'cpp', default_options : ['cpp_std=c++20'])

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
  add_global_arguments('-DDEBUG -fsanitize=address', language : 'cpp')
endif

tyche_include_dir = include_directories('.')

spdlog = subproject('spdlog')
spdlog_dep = dependency('spdlog')
tomlplusplus = subproject('tomlplusplus')
tomlplusplus_dep = dependency('tomlplusplus')

subdir('tyche')
subdir('test')
