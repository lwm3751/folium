import os
import sys

if sys.platform == "win32":
    cpppath = [r'D:\toolbox\boost_1_36_0', r'D:\Python26\include']
    libpath = [r'D:\toolbox\boost_1_36_0\lib_x86\lib', r'D:\Python26\libs']
    env = Environment(tools=['mingw'], CPPPATH=cpppath, LIBPATH=libpath)
    ccflags = '-O3'
    libs=['boost_python-mgw34-mt', 'python26']
else:
    env = Environment()
    ccflags = '-O3'

cppdefines = {'NDEBUG':None}

src = []
for root, dirs, files in os.walk('cpp'):
    for file in files:
        if file.endswith('.cpp'):
            src.append(os.path.join(root, file))

folium = env.Program(
    'folium',
    src,
    LIBS=libs,
    CPPDEFINES = cppdefines,
    CCFLAGS = ccflags)
