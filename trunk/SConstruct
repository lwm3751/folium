import os
import sys

if sys.platform == "win32":
    CPPPATH = [r'D:\toolbox\boost_1_36_0']
    LIBPATH = [r'D:\toolbox\boost_1_36_0\lib_x86\lib']
    env = Environment(tools=['mingw'], CPPPATH=CPPPATH, LIBPATH=LIBPATH)
    CCFLAGS = '-O3'
    LIBS=['boost_thread-mgw34-mt']
else:
    env = Environment()
    CCFLAGS = '-O3'

CPPDEFINES = {'NDEBUG':None}

src = []
for root, dirs, files in os.walk('cpp'):
    for file in files:
        if file.endswith('.cpp'):
            src.append(os.path.join(root, file))

folium = env.Program(
    'folium',
    src,
    LIBS=LIBS,
    CPPDEFINES = CPPDEFINES,
    CCFLAGS = CCFLAGS)
