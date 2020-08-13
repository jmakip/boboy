project('boboy', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


sdldep = dependency('sdl2', version : '>=2.0.0')

incdir = include_directories('include')
boboy_src = ['src/boboy.c', 'src/cart.c', 'src/cpu.c', 'src/gpu.c', 'src/mem.c','src/sdl.c', 'src/timer.c']

exe = executable('boboy', boboy_src,
  install : true, include_directories : incdir, dependencies : sdldep)

#test('basic', exe)

subdir('test')
