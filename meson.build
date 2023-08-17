project('tyche', 'cpp')

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
  add_global_arguments('-DDEBUG', language : 'cpp')
endif

tyche_include_dir = include_directories('.')

spdlog_dep = dependency('spdlog')

subdir('tyche')
subdir('test')
