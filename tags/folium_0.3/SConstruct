import sys
cpp_path = ['#include']
lib_path = ['#bin']
env = Environment(CPPPATH=cpp_path, LIBPATH=lib_path)

if sys.platform == "win32":
    env.Tool('mingw')
ccflag = '-O3'
ccflag = '-O3 -march=prescott'
defines = {'NDEBUG':None}
Export("env")
Export("ccflag")
Export("defines")
env.SConscript('src/sconscript', build_dir='build/folium', duplicate=0)
env.SConscript('qianhong/sconscript', build_dir='build/qianhong', duplicate=0)

