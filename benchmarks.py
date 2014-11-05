from __future__ import print_function
import timeit
import random
import skiplist


setup_tmpl = """
from __main__ import {0}
"""


def timer(fn, n=100):
    setup = setup_tmpl.format(fn.__name__)
    print('%s:' % fn.__name__)
    print(timeit.timeit('%s()' % fn.__name__, setup=setup, number=n))


RANDOMLONGS = [random.randint(1, 500000) for i in range(1000)]


def sort_nearly_sortedlist():
    s = []
    for i in RANDOMLONGS:
        s.append(i)
        s.sort()


def skiplist_add():
    s = skiplist.SortedSet()
    for i in RANDOMLONGS:
        s.add(i)


if __name__ == '__main__':
    timer(sort_nearly_sortedlist)
    timer(skiplist_add)
