project('xtea-flavors', 'cpp',
    license: [ 'mit' ],
    default_options: [ 'b_lto=true', 'cpp_std=c++17' ]
)

headers = files('base.h', 'avx.h', 'ocl.h', 'ref.h', 'stl.h')
sources = files('avx.cpp', 'ocl.cpp', 'ref.cpp', 'stl.cpp', 'main.cpp')

opencl_dep = dependency('OpenCL')
tbb_dep = dependency('TBB')

run_target('format',
    command: ['clang-format', '-i', '-style=llvm', headers, sources]
)

xtea_flavors = executable('xtea-flavors', sources, 
    cpp_args: '-march=native',
    #cpp_args: ['-march=native', '-fdata-sections', '-ffunction-sections', '-fno-plt', '-fvisibility=hidden'],
    link_args: '-Wl,--sort-common,-z,relro,-z,now',
    dependencies : [opencl_dep, tbb_dep]
)
