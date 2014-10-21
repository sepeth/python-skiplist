from ._sortedset import SortedSet

__all__ = ['SortedDict', 'SortedSet']


class Pair(object):
    def __init__(self, key, value=None):
        self.key = key
        self.value = value

    def __lt__(self, other):
        return self.key < other.key

    def __eq__(self, other):
        return self.key == other.key

    def __repr__(self):
        return "%r: %r" % (self.key, self.value)


class SortedDict(SortedSet):
    def __init__(self, items=None):
        items = map(lambda x: Pair(*x), items if items else [])
        super(SortedDict, self).__init__(items)

    def __getitem__(self, key):
        try:
            return super(SortedDict, self).__getitem__(Pair(key)).value
        except KeyError:
            raise KeyError('%s is not in the SortedDict' % key)

    def __setitem__(self, key, value):
        super(SortedDict, self).add(Pair(key, value))

    def __delitem__(self, key):
        try:
            self.remove(Pair(key))
        except KeyError:
            raise KeyError('%s is not in the SortedDict' % key)
