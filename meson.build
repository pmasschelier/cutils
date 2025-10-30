project('cutils', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'c_std=c23',
  ]
)

# Make this library usable as a Meson subproject.
cutils_dep = declare_dependency(include_directories: 'include')

if not meson.is_subproject()
  subdir('test')
endif