from ._sortedset import SortedSet


class Pair(object):
    def __init__(self, key, value=None):
        self.key = key
        self.value = value

    def __lt__(self, other):
        return self.key < other.key

    def __eq__(self, other):
        return self.key == other.key


class SortedDict(SortedSet):
    def __getitem__(self, key):
        return super(SortedDict, self).__getitem__(Pair(key)).value

    def __setitem__(self, key, value):
        super(SortedDict, self).add(Pair(key, value))
