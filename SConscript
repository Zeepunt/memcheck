from building import *
import os

cwd = GetCurrentDir()
group = []
src = Glob('src/*.c')

CPPPATH = [os.path.join(cwd, 'inc')]

list = os.listdir(cwd)

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        group = group + SConscript(os.path.join(d, 'SConscript'))

group = group + DefineGroup('memcheck', src, depend=[''], CPPPATH=CPPPATH)
Return('group')