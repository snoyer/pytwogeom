import os
import subprocess
import setuptools
from setuptools.command.install_lib import install_lib


def setup() :
    lib2geom_include = find_include_dir_for('2geom/2geom.h')
    lib2geom_dir, lib2geom_name = locate_best('lib2geom.so', 'lib2geom_LIB.so')
    lib2geom_libname = os.path.basename(lib2geom_name).lstrip('lib').rstrip('.so')

    class InstallLibCommand(install_lib):
        """write file containing library path used at compile time"""
        def run(self):
            install_lib.run(self)
            txt_path = os.path.join(self.install_dir, 'pytwogeom', 'ld_library_path')
            with open(txt_path, 'w') as out:
                out.write(lib2geom_dir)

    setuptools.setup(
        name='pytwogeom',
        version='0.0.1',
        description='lib2geom bindings',
        url='http://github.com/snoyer/pytwogeom',
        author='snoyer',
        author_email='reach me on github',
        packages=['pytwogeom'],
        install_requires=[],
        ext_modules=[
            setuptools.Extension(
                'pytwogeom._pytwogeom',
                ['src/pytwogeom.cpp'],
                include_dirs=['src', lib2geom_include],
                library_dirs=[lib2geom_dir],
                libraries=['boost_python', lib2geom_libname, 'cairo', 'gsl', 'glib-2.0'],
                language='c++11',
            ),
        ],
        cmdclass = {
            'install_lib': InstallLibCommand,
        },
    )



def locate(*names) :
    p = subprocess.Popen(['locate']+list(names), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out,err = p.communicate()
    for path in out.decode('utf8').splitlines() :
        for i,name in enumerate(names) :
            if path.endswith(name) :
                dirname = path[:-len(name)]
                if os.path.isdir(dirname) :
                    yield dirname, name

def locate_best(*names) :
    match_priority = {k:i for i,k in enumerate(names)}
    def score(path_name) :
        path,name = path_name
        folders = os.path.normpath(path).split(os.sep)
        return (
            match_priority.get(name),
            0 if 'include' in folders else 1,
            len([f for f in folders if f.startswith('.')]),
            len(folders),
        )

    return min(locate(*names), key=score)

def find_include_dir_for(*names) :
    dirname,name = locate_best(*names)
    return dirname

setup()
