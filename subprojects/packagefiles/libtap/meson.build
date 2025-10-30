project('libtap', 'c', version: '0.1.0', license: 'LGPL-3.0-only')

tap_lib = static_library('tap', 'tap.c')
tap_inc = include_directories('.')

libtap_dep = declare_dependency(
  link_with: tap_lib,
  include_directories: tap_inc
)
