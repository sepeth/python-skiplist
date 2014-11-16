python-skiplist
===========

[![Build Status](https://travis-ci.org/sepeth/python-skiplist.svg?branch=master)](https://travis-ci.org/sepeth/python-skiplist)
[![Coverage Status](https://img.shields.io/coveralls/sepeth/python-skiplist.svg)](https://coveralls.io/r/sepeth/python-skiplist?branch=master)
[![Code Health](https://landscape.io/github/sepeth/python-skiplist/master/landscape.png)](https://landscape.io/github/sepeth/python-skiplist/master)

Skip Lists are data structure that can be used in place of balanced trees. They
are easier to implement and generally faster. This library uses skip lists to
implement SortedDict and SortedSet data types for Python.

SortedSet is implemented in C and it uses CPython C API, and SortedDict is a thin
wrapper on top of SortedSet.

Here is a few examples:

```python
>>> from skiplist import SortedSet, SortedDict
>>> d = SortedDict({'elma': 1, 'armut': 2, 'kel': 3, 'mahmut': 4})
>>> d
SortedDict({'armut': 2, 'elma': 1, 'kel': 3, 'mahmut': 4})
>>> del d['kel']
>>> d
SortedDict({'armut': 2, 'elma': 1, 'mahmut': 4})
>>> cities = SortedSet(['Canakkale', 'Zurih', 'Izmir', 'Bolu', 'Istanbul'])
>>> cities
SortedSet(['Bolu', 'Canakkale', 'Istanbul', 'Izmir', 'Zurih'])
```

Installation
------------

```bash
$ pip install python-skiplist
```

Asymptotic Bounds
-----------------

SortedSet Operations | Average Case
-------------------- | ------------
add                  | O(log N)
discard              | O(log N)
contains             | O(log N)
length               | O(1)


SortedDict Operations | Average Case
--------------------- | ------------
\__setitem__           | O(log N)
\__getitem__           | O(1)
\__delitem__           | O(log N)
