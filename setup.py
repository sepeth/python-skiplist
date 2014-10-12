from distutils.core import setup, Extension

setup(name='skiplist',
      version='1.0',
      description='Fast Skiplist Implementation',
      ext_modules=[Extension('skiplist', ['skiplist.c'])])
