skiplist.py
===========

[![Build Status](https://travis-ci.org/sepeth/skiplist.py.svg?branch=master)](https://travis-ci.org/sepeth/skiplist.py)
[![Coverage Status](https://img.shields.io/coveralls/sepeth/skiplist.py.svg)](https://coveralls.io/r/sepeth/skiplist.py?branch=master)

Skip Lists are data structure that can be used in place of balanced trees. They
are easier to implement and generally faster. This library uses skip lists to
implement SortedDict and SortedSet data types for Python.

SortedSet is implemented in C and it uses CPython C API, and SortedDict is a thin
wrapper on top of SortedSet.

Here is a few examples:

```python
>>> from skiplist import SortedSet, SortedDict
>>> d = SortedDict({'elma': 1, 'armut': 2, 'kel': 3, 'mahmut': 4})
SortedDict({'armut': 2, 'elma': 1, 'kel': 3, 'mahmut': 4})

>>> del d['kel']
SortedDict({'armut': 2, 'elma': 1, 'mahmut': 4}) 

>>> cities = SortedSet(['Zurih', 'Izmir', 'Istanbul', 'Ankara'])
>>> cities
SortedSet(['Ankara', 'Istanbul', 'Izmir', 'Zurih'])
```
