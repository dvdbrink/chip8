import os
import platform

# Build GDL
if os.path.exists('external/gdl/SConstruct'):
    SConscript('external/gdl/SConstruct')
else:
    print('GDL not found. Clone GDL in ./external.')
    Exit(1)

# Initialize environment
if platform.system() == 'Linux':
    env = Environment()
elif platform.system() == 'Windows':
    env = Environment(tools = ['mingw'])
else:
    print('Error: Unsupported platform (' + platform.system() + ')')
    Exit(1)

env.Append(
    BINDIR = '#build/bin',
    LIBDIR = '#build/bin',
    OBJDIR = '#build/obj',
    CPPPATH = [
        Glob('include'),
        Glob('external/gdl/include')
    ],
    CPPFLAGS = [
        '-std=c++11',
        '-Wall',
        '-Wextra',
    ]
)

# Build program
env.Append(
    LIBS = ['gdl-oal', 'gdl-ogl', 'gdl-sys'],
    LIBPATH = env['LIBDIR'],
    RPATH = env.Literal('\\$$ORIGIN')
)

env.VariantDir(env['OBJDIR'], 'source', duplicate = 0)
env.Program(os.path.join(env['BINDIR'], 'chip8'), Glob(os.path.join(env['OBJDIR'], '*.cpp')))

# Symlink resource folder in bin directory (Linux only)
if platform.system() == 'Linux':
    if not os.path.exists(Dir('#').abspath + '/build/bin/resource'):
        if not os.path.exists(Dir('#').abspath + '/build/bin'):
            os.makedirs(Dir('#').abspath + '/build/bin')
        os.symlink(Dir('#').abspath + '/resource', Dir('#').abspath + '/build/bin/resource')
