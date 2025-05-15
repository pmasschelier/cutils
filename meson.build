project('cutils', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'c_std=c23',
  ]
)

src = files(
  'include/cutils/arena.h'
)

lib_args = ['-DBUILDING_CUTILS']

lib = static_library('cutils', src,
  install : false,
  c_args : lib_args,
  gnu_symbol_visibility : 'hidden',
  include_directories: 'include'
)

test_exe = executable('arena_test', 'test/test_arena.c',
  link_with : lib, include_directories: 'include')
test('arena', test_exe)
test_exe = executable('array_list_test', 'test/test_array_list.c',
  link_with : lib, include_directories: 'include')
test('array_list', test_exe)

# Make this library usable as a Meson subproject.
cutils_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with : lib)

