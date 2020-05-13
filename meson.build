project('boboy', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

incdir = include_directories('include')
boboy_sources = ['boboy.c', 'cart.c', 'cpu.c', 'mem.c', 'gpu.c']

exe = executable('boboy', boboy_sources,
  install : true, include_directories : incdir)

test('basic', exe)
