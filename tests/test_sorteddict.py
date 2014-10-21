import unittest
from skiplist import SortedDict


class TestSortedDictSubscript(unittest.TestCase):
    def test_raise_keyerror_for_nonexisting_key(self):
        s = SortedDict()
        self.assertRaises(KeyError, s.__getitem__, 'anaktar')

    def test_setitem_and_getitem(self):
        s = SortedDict()
        s[3] = 'somevalue'
        self.assertEqual(s[3], 'somevalue')


class TestSortedDictDel(unittest.TestCase):
    def test_raise_keyerror_for_deletion_nonexisting_key(self):
        s = SortedDict()
        with self.assertRaises(KeyError):
            del s['anaktar']

    def test_deletion_of_an_element_in_the_dict(self):
        s = SortedDict()
        s['elma'] = 1
        s['armut'] = 2
        s['kel'] = 3
        s['mahmut'] = 4
        oldlen = len(s)
        del s['elma']
        self.assertEqual(len(s), oldlen - 1)
        with self.assertRaises(KeyError):
            del s['elma']


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
