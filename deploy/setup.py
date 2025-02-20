
# Generated using: python setup_py_configure.py 'sgext'

from __future__ import print_function
from os import sys, path
from skbuild import setup

sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
from sgextVersion import get_versions

# this_directory = path.abspath(path.dirname(__file__))
# sgext_readme_path = path.join(this_directory, 'SGEXT-source', 'SGEXT', 'README.md')
# if path.exists(sgext_readme_path):
#     with open(sgext_readme_path, encoding='utf-8') as f:
#         long_description = f.read()
# else:
#     with open(path.join(this_directory, 'README.md'), encoding='utf-8') as f:
#         long_description = f.read()

long_description= r'SGEXT is an open-source, cross-platform library for ' \
                   'skeletonization of vascular/filaments images and tools for the posterior ' \
                   'graph analysis, using a spatial graph as an encriched graph with geometric ' \
                   'information.'
setup(
    name='sgext',
    version=get_versions()['package-version'],
    author='Pablo Hernandez-Cerdan',
    author_email='pablo.hernandez.cerdan@outlook.com',
    packages=['sgext'],
    package_dir={'sgext': 'sgext'},
    package_data={
        'sgext': ['tables/*.zlib']
    },
    cmake_source_dir='../',
    cmake_args=[
        '-DSG_BUILD_TESTING:BOOL=OFF',
        '-DSG_MODULE_ANALYZE:BOOL=ON',
        '-DSG_MODULE_COMPARE:BOOL=ON',  # VTK
        '-DSG_MODULE_LOCATE:BOOL=ON',  # VTK
        '-DSG_MODULE_GENERATE:BOOL=ON', # C++17, optional TBB (recommended)
        '-DSG_MODULE_DYNAMICS:BOOL=ON', # C++17
        '-DSG_MODULE_SCRIPTS:BOOL=ON',  # ITK
        '-DSG_BUILD_CLI:BOOL=ON',  # ITK
        '-DSG_MODULE_VISUALIZE:BOOL=ON',  # VTK
        '-DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF'  # QT
        '-DSG_WRAP_PYTHON:BOOL=ON'
    ],
    cmake_install_target="sgext-install-runtime",
    py_modules=[
        'sgextVersion',
    ],
    download_url=r'https://github.com/phcerdan/SGEXT',
    description=r'SGEXT is an open-source toolkit for skeletonization and graph analysis',
    long_description=long_description,
    long_description_content_type='text/markdown',
    classifiers=[
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Healthcare Industry",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Medical Science Apps.",
        "Topic :: Scientific/Engineering :: Information Analysis",
        "Topic :: Software Development :: Libraries",
        "Operating System :: Android",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Operating System :: Unix",
        "Operating System :: MacOS"
        ],
    license='GPLv3',
    keywords=r'SGEXT thinning skeletonization graph complex-networks imaging',
    url=r'https://github.com/phcerdan/SGEXT',
    install_requires=[
    ]
    )
