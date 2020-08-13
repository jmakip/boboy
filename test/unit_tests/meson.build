unity_dep = dependency('unity', fallback : ['unity', 'unity_dep'])

test_runner1 = executable('runner1', 
  ['test1.c', 'test1_runner.c', '../../src/mem.c'],
  dependencies: [unity_dep],
  include_directories: incdir,
)
test('unity test 1', test_runner1, args : [test_runner1], suite: 'tts_unity')
  


#link_args: ['-Wl,--wrap=exit', '-Wl,--wrap=ptrace', '-Wl,--wrap=execv']
