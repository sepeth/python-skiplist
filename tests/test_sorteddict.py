import unittest
from skiplist import SortedDict


class TestSortedDictRepr(unittest.TestCase):
    def test_empty_sorteddict_repr(self):
        s = SortedDict()
        self.assertEqual("SortedDict()", repr(s))

    def test_sorteddict_repr_with_some_elements_in_it(self):
        s = SortedDict()
        s['elma'] = 1
        s['armut'] = 2
        s['kel'] = 3
        s['mahmut'] = 4
        self.assertEqual(
            "SortedDict({'armut': 2, 'elma': 1, 'kel': 3, 'mahmut': 4})",
            repr(s)
        )


if __name__ == '__main__':
    unittest.main()
