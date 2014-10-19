from setuptools import setup, find_packages, Extension


setup(
    name='skiplist',
    version='0.1',
    description='SortedSet and SortedDict implementation by using skiplist',
    url='https://github.com/sepeth/skiplist.py',
    author='Doğan Çeçen',
    author_email='sepeth@gmail.com',

    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Python Software Foundation License',
        'Programming Language :: Python :: 3.4',
    ],

    packages=find_packages(exclude=['tests*']),
    test_suite='tests',

    ext_modules=[
        Extension('skiplist._sortedset', ['skiplist/sortedsetobject.c']),
    ]
)
