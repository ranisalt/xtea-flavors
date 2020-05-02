project('xtea-flavors', 'cpp',
    license: [ 'mit' ],
    default_options: [ 'cpp_std=c++17' ]
)

headers = files('base.h', 'ocl.h', 'ref.h', 'stl.h')
sources = files('ocl.cpp', 'ref.cpp', 'stl.cpp', 'main.cpp')

opencl_dep = dependency('OpenCL')
openmp_dep = dependency('OpenMP')
tbb_dep = dependency('TBB')

run_target('format',
    command: ['clang-format', '-i', '-style=llvm', headers, sources]
)

xtea_flavors = executable('xtea-flavors', sources, 
    dependencies : [opencl_dep, openmp_dep, tbb_dep]
)
