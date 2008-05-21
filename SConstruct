import sys
cpp_path = [r'D:\toolbox\boost_1_35_0']
lib_path = ['#bin', r'D:\toolbox\boost_1_35_0\stage\lib']

BuildDir('build', 'src', duplicate=0)

env = Environment(CPPPATH=cpp_path, LIBPATH=lib_path)

if sys.platform == "win32":
	if 1:
		env.Tool('mingw')
		ccflag = '-O3'
	else:
		env.Tool('icl')
		ccflag = '/Ox /MD'
else:
	ccflag = '-O3'

defines = {'NDEBUG':None}

src = ['build/engine_data.cpp',
    'build/move_gen.cpp',
    'build/xq_data.cpp',
    'build/bitmap_data.cpp',
    'build/generator.cpp',
    'build/hash.cpp',
    'build/search.cpp',
    'build/engine.cpp',
    'build/history_data.cpp',
    'build/xq.cpp',
    'build/move.cpp',
	'build/qianhong/qianhong.cpp']

folium = env.Program(
    'folium',
    src,
    LIBS=['boost_thread-mgw34-mt'],
    CPPDEFINES = defines,
    CCFLAGS = ccflag)

