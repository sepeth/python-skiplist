from ._sortedset import SortedSet, Pair

__all__ = ['SortedDict', 'SortedSet']


try:
    from itertools import imap
except ImportError:
    imap = map


class SortedDict(SortedSet):
    def __init__(self, items=None, **kwargs):
        if isinstance(items, dict):
            items = items.items()
        items = map(lambda x: Pair(*x), items if items else [])
        super(SortedDict, self).__init__(items)
        for k, v in kwargs.items():
            self[k] = v

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

    def __contains__(self, key):
        return super(SortedDict, self).__contains__(Pair(key))

    def __iter__(self):
        return imap(lambda x: x.key, super(SortedDict, self).__iter__())

    def __repr__(self):
        if len(self) == 0:
            return '%s()' % type(self).__name__
        l = map(lambda x: repr(x), super(SortedDict, self).__iter__())
        return '%s({%s})' % (type(self).__name__, ', '.join(l))

    def items(self):
        return map(lambda x: x.as_tuple(), super(SortedDict, self).__iter__())

    def keys(self):
        return map(lambda x: x.key, super(SortedDict, self).__iter__())

    def values(self):
        return map(lambda x: x.value, super(SortedDict, self).__iter__())
