import sys
cpp_path = ['#include']
lib_path = ['#bin']
env = Environment(CPPPATH=cpp_path, LIBPATH=lib_path)

if sys.platform == "win32":
    env.Tool('mingw')
Export("env")
env.SConscript('src/sconscript', build_dir='build/folium', duplicate=0)
env.SConscript('qianhong/sconscript', build_dir='build/qianhong', duplicate=0)

