import sys
cpp_path = ['#include']
lib_path = ['#bin']
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
Export("env")
Export("ccflag")
Export("defines")
env.SConscript('src/sconscript', build_dir='build/folium', duplicate=0)
env.SConscript('qianhong/sconscript', build_dir='build/qianhong', duplicate=0)
