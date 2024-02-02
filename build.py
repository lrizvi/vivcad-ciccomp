from pybind11.setup_helpers import Pybind11Extension, build_ext

def build(setup_kwargs):
    ext_modules = [
        Pybind11Extension(
            'ciccomp',
            [
                'src/band.cpp',
                'src/barycentric.cpp',
                'src/cheby.cpp',
                'src/eigenvalue.cpp',
                'src/main.cpp',
                'src/pm.cpp',
            ],
            include_dirs = [
                'include',
            ],
            extra_compile_args = [
                '-O3'
            ],
            extra_link_args = [
                '-l:libgomp.a',
                '-l:mpfr.a'
            ],
            language = 'c++',
            cxx_std = 11
        ),
    ]
    setup_kwargs.update({
        'ext_modules': ext_modules,
        'cmd_class'  : {'build_ext': build_ext},
        'zip_safe'   : False,
    })

    setuptools.setup(
        **setup_kwargs, 
        script_args = ['bdist_wheel'],
        options = { 
            'bdist_wheel': {
                'plat_name': os.getenv('PP_PYTHON_TARGET', 'any')
            }
        }
    )
