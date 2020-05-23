project('boboy', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


sdldep = dependency('sdl2', version : '>=2.0.0')

incdir = include_directories('include')
boboy_sources = ['boboy.c', 'cart.c', 'cpu.c', 'gpu.c', 'mem.c','sdl.c', 'timer.c']

exe = executable('boboy', boboy_sources,
  install : true, include_directories : incdir, dependencies : sdldep)

test('basic', exe)
