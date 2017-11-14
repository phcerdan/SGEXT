import os
import ycm_core

#From: https://github.com/oblitum/dotfiles/blob/archlinux/.ycm_extra_conf.py

# flags = [
# '-x', 'c++',
# '-Wall',
# '-Wextra',
# '-pedantic',
# '-std=c++11',
# '-stdlib=libc++',
# '-isystem', '/usr/include/c++/5.3.0',
# '-isystem', '/usr/include/c++/5.3.0/x86_64-unknown-linux-gnu',
# '-isystem', '/usr/local/include',
# '-isystem', '/usr/lib/clang/3.7.1/include',
# '-isystem', '/usr/include',
# '-isystem', '/home/phc/devtoolset/release/ITK/include/ITK-4.9',
# '-isystem', '/home/phc/devtoolset/release/include',
# '-isystem', '/usr/include/GraphicsMagick',
# '-isystem', '/usr/include/cairo',
# '-isystem', '/usr/local/include',
# '-isystem', '/usr/include/eigen3',
# '-isystem', '/home/phc/Software/DGtal/src-fork/src',
# '-isystem', '/home/phc/Software/DGtal/src-fork/tests',
# '-isystem', '/home/phc/Software/DGtal/fork-build/src',
# '-isystem', '/home/phc/Software/DGtal/fork-build/tests',
# '-isystem', '/usr/include/qt',
# '-isystem', '/usr/include/qt/QtWidgets',
# '-isystem', '/usr/include/qt/QtGui',
# '-isystem', '/usr/include/qt/QtCore',
# '-isystem', '/usr/lib/qt/mkspecs/linux-g++',
# '-isystem', '/usr/include/qt/QtOpenGL',
# '-isystem', '/usr/include/qt/QtXml',
# ]
flags = [
'-x',
'c++',
'-std=c++11',
'-Wall',
'-Wextra',
'-pedantic',
'-DCGAL_USE_EIGEN3',
'-DCGAL_USE_GMP',
'-DCGAL_USE_MPFR',
'-DCPP11_ARRAY',
'-DCPP11_AUTO',
'-DCPP11_FORWARD_LIST',
'-DCPP11_INITIALIZER_LIST',
'-DCPP11_RREF_MOVE',
'-DCPP11_UNORDERED_MAP',
'-DCPP11_UNORDERED_SET',
'-DGMP_HAS_IOSTREAM',
'-DITK_IO_FACTORY_REGISTER_MANAGER',
'-DQT_CORE_LIB',
'-DQT_GUI_LIB',
'-DQT_NO_DEBUG',
'-DQT_OPENGL_LIB',
'-DQT_WIDGETS_LIB',
'-DQT_XML_LIB',
'-DUNIX',
'-DWITH_QT5',
'-DvtkRenderingCore_AUTOINIT=3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL)',
'-I/home/phc/Software/DGtal/fork/ITKIOFactoryRegistration',
'-I/usr/include/freetype2',
'-I/usr/include/vtk',
'-I/opt/cuda/include/CL',
'-I/home/phc/devtoolset/release/ITK/include/ITK-4.9',
'-isystem', '/home/phc/devtoolset/release/include',
'-I/usr/include/GraphicsMagick',
'-I/usr/include/cairo',
'-I/usr/local/include',
'-isystem', '/usr/include/eigen3',
'-I/home/phc/Software/DGtal/src-fork/src',
'-I/home/phc/Software/DGtal/src-fork/tests',
'-I/home/phc/Software/DGtal/build',
'-I/home/phc/Software/DGtal/build/src',
'-I/home/phc/Software/DGtal/build/tests',
'-isystem', '/usr/include/qt',
'-isystem', '/usr/include/qt/QtWidgets',
'-isystem', '/usr/include/qt/QtGui',
'-isystem', '/usr/include/qt/QtCore',
'-isystem', '/usr/lib/qt/mkspecs/linux-g++',
'-isystem', '/usr/include/qt/QtOpenGL',
'-isystem', '/usr/include/qt/QtXml',
'-march=x86-64', '-mtune=generic', '-O2', '-pipe', '-fstack-protector-strong', '-frounding-math',
'-std=c++11', '-msse2', '-fopenmp', '-O3', '-DNDEBUG',
'-DWITH_C11', '-DWITH_GMP', '-DWITH_MAGICK', '-DWITH_ITK', '-DWITH_CAIRO', '-DWITH_HDF5', '-DWITH_VISU3D_QGLVIEWER', '-DWITH_OPENMP', '-DWITH_EIGEN',
'-DCGAL_EIGEN3_ENABLED', '-DWITH_CGAL', '-DWITH_Eigen3', '-DWITH_LAPACK',
'-fPIC'
]
#Template declaration in .ih files:
#From: https://github.com/Valloric/YouCompleteMe/issues/1938#issuecomment-175411991
def GetIncludeTemplateDeclaration( filename ):
    root, ext = os.path.splitext( filename )
    if ext == '.ih':
        return [ '-include', root + '.h' ]
    return []


def FlagsForFile( filename, **kwargs ):
    flags.extend( GetIncludeTemplateDeclaration( filename ) )
    #Prepend: (no changes)
    #flags[:0] = GetIncludeTemplateDeclaration( filename )

    return {
        'flags': flags,
        'do_cache': True
    }

# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
# compilation_database_folder = '/home/phc/Software/DGtal/fork-build'
compilation_database_folder = ''

if os.path.exists( compilation_database_folder ):
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ]


def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
  if not working_directory:
    return list( flags )
  new_flags = []
  make_next_absolute = False
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]
  for flag in flags:
    new_flag = flag

    if make_next_absolute:
      make_next_absolute = False
      if not flag.startswith( '/' ):
        new_flag = os.path.join( working_directory, flag )

    for path_flag in path_flags:
      if flag == path_flag:
        make_next_absolute = True
        break

      if flag.startswith( path_flag ):
        path = flag[ len( path_flag ): ]
        new_flag = path_flag + os.path.join( working_directory, path )
        break

    if new_flag:
      new_flags.append( new_flag )
  return new_flags


def IsHeaderFile( filename ):
  extension = os.path.splitext( filename )[ 1 ]
  return extension in [ '.h', '.hxx', '.hpp', '.hh', '.ih' ]


def GetCompilationInfoForFile( filename ):
  # The compilation_commands.json file generated by CMake does not have entries
  # for header files. So we do our best by asking the db for flags for a
  # corresponding source file, if any. If one exists, the flags for that file
  # should be good enough.
  if IsHeaderFile( filename ):
    basename = os.path.splitext( filename )[ 0 ]
    for extension in SOURCE_EXTENSIONS:
      replacement_file = basename + extension
      if os.path.exists( replacement_file ):
        compilation_info = database.GetCompilationInfoForFile(
          replacement_file )
        if compilation_info.compiler_flags_:
          return compilation_info
    return None
  return database.GetCompilationInfoForFile( filename )


# def FlagsForFile( filename, **kwargs ):
#   if database:
#     # Bear in mind that compilation_info.compiler_flags_ does NOT return a
#     # python list, but a "list-like" StringVec object
#     compilation_info = GetCompilationInfoForFile( filename )
#     if not compilation_info:
#       return None
#
#     final_flags = MakeRelativePathsInFlagsAbsolute(
#       compilation_info.compiler_flags_,
#       compilation_info.compiler_working_dir_ )
#   else:
#     relative_to = DirectoryOfThisScript()
#     final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )
#
#   return {
#     'flags': final_flags,
#     'do_cache': True
#   }
