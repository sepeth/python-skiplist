from __future__ import print_function
from contextlib import contextmanager
import time
import random
import skiplist
import gc


@contextmanager
def timeit(name):
    oldgc = gc.isenabled()
    gc.disable()
    print('%s:' % name)
    t1 = time.time()
    yield
    t2 = time.time()
    if oldgc:
        gc.enable()
    print(t2 - t1)


RANDOMLONGS = [random.randint(1, 500000) for i in range(10000)]


def sort_nearly_sortedlist():
    s = []
    with timeit('append to a list and sort it each time'):
        for i in RANDOMLONGS:
            s.append(i)
            s.sort()


def sortedset_add():
    s = skiplist.SortedSet()
    with timeit('add operation of SortedSet'):
        for i in RANDOMLONGS:
            s.add(i)


if __name__ == '__main__':
    sort_nearly_sortedlist()
    sortedset_add()
