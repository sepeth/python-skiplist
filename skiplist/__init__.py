from ._sortedset import BaseSortedSet
from collections import MutableMapping, MutableSet

__all__ = ['SortedDict', 'SortedSet']


class SortedSet(BaseSortedSet, MutableSet):
    pass


class SortedDict(MutableMapping):
    def __init__(self, items=None, **kwargs):
        self._sortedkeys = BaseSortedSet()
        self._map = {}
        if items is None:
            items = []
        elif isinstance(items, dict):
            items = items.items()
        for k, v in items:
            self[k] = v
        for k, v in kwargs.items():
            self[k] = v

    def __getitem__(self, key):
        return self._map[key]

    def __setitem__(self, key, value):
        if key not in self._map:
            self._sortedkeys.add(key)
        self._map[key] = value

    def __delitem__(self, key):
        if key not in self._map:
            raise KeyError('%s is not in the SortedDict' % key)
        self._sortedkeys.discard(key)
        del self._map[key]

    def __len__(self):
        return len(self._map)

    def __iter__(self):
        return iter(self._sortedkeys)

    def __repr__(self):
        if len(self) == 0:
            return '%s()' % type(self).__name__
        l = ('%r: %r' % (k, v) for k, v in self.items())
        return '%s({%s})' % (type(self).__name__, ', '.join(l))
