project('rs2-c', 'c',
  version : '0.1',
  license: 'GPL-3.0-or-later',
  default_options : ['c_std=c11', 'warning_level=3'])


add_global_arguments(['-Wno-unused-const-variable',
                      '-Wno-unused-parameter'
                      ],
                      language : 'c')

incdir = include_directories('include')

subdir('src')
subdir('src/collection')
subdir('src/def')
subdir('src/scene')

cc = meson.get_compiler('c')

math_dep = cc.find_library('m', required : true)
sdl2_dep = dependency('sdl2', version : '>=2.0.18')

deps = [dependency('libsodium'), dependency('SDL2_ttf'), math_dep, sdl2_dep]

exe = executable(
  'rs2client317', sources,
  dependencies : deps,
  include_directories : incdir)

