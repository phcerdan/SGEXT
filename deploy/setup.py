
# Generated using: python setup_py_configure.py 'sgext'

from __future__ import print_function
from os import sys, path

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.', file=sys.stderr)
    print('Please run:', file=sys.stderr)
    print('', file=sys.stderr)
    print('  python -m pip install scikit-build')
    sys.exit(1)

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

long_description= r'SGEXT is an open-source, cross-platform library for '
                   'skeletonization of vascular/filaments images and tools for the posterior '
                   'graph analysis, using a spatial graph as an encriched graph with geometric '
                   'information.'
setup(
    name='sgext',
    version=get_versions()['package-version'],
    author='Pablo Hernandez-Cerdan',
    author_email='pablo.hernandez.cerdan@outlook.com',
    packages=['sgext'],
    package_dir={'sgext': 'sgext'},
    cmake_args=[],
    py_modules=[
        'sgextVersion',
        'sgextBuildOptions'
    ],
    download_url=r'https://github.com/phcerdan/SGEXT',
    description=r'SGEXT is an open-source toolkit for skeletonization and graph analysis',
    long_description=long_description,
    long_description_content_type='text/markdown',
    classifiers=[
        "License :: OSI Approved :: Mozilla Public License 2.0 (MPL 2.0)",
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
    license='MPL2',
    keywords=r'SGEXT thinning skeletonization graph complex-networks imaging'
    url=r'https://github.com/phcerdan/SGEXT',
    install_requires=[
    ]
    )
