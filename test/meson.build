fs = import('fs')

sources = files(
    'arena_static.c',
    'arena_dynamic.c',
    'basic_array.c',
)

libtap = dependency('libtap')

foreach src : sources
    name = fs.stem(src)
    exe = executable(name, src, dependencies: [libtap, cutils_dep])
    test(name, exe, protocol: 'tap')
endforeach