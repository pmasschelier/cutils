fs = import('fs')

sources = files(
    'arena_basic.c',
    'array_basic.c',
    'bump_basic.c',
)

libtap = dependency('libtap')

foreach src : sources
    name = fs.stem(src)
    exe = executable(name, src, dependencies: [libtap, cutils_dep])
    test(name, exe, protocol: 'tap')
endforeach