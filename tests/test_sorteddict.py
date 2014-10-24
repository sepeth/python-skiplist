import unittest
from skiplist import SortedDict


class TestSortedDictInit(unittest.TestCase):
    def test_with_tuples(self):
        s = SortedDict([('elma', 1), ('armut', 2), ('kel', 3), ('mahmut', 4)])
        self.assertEqual(len(s), 4)
        self.assertEqual(s['kel'], 3)

    def test_with_kwargs(self):
        s = SortedDict(elma=1, armut=2, kel=3, mahmut=4)
        self.assertEqual(len(s), 4)
        self.assertEqual(s['kel'], 3)


class TestSortedDictSubscript(unittest.TestCase):
    def test_raise_keyerror_for_nonexisting_key(self):
        s = SortedDict()
        with self.assertRaises(KeyError):
            s['anaktar']

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


class TestSortedDictContains(unittest.TestCase):
    def setUp(self):
        self.sd = SortedDict([('elma', 1), ('armut', 2)])

    def test_in_operator_for_existing_element(self):
        self.assertIn('armut', self.sd)

    def test_in_operator_for_nonexisting_element(self):
        self.assertNotIn('kel', self.sd)


class TestSortedDictIterMethods(unittest.TestCase):
    def setUp(self):
        self.sd = SortedDict([
            ('elma', 1),
            ('armut', 2),
            ('kel', 3),
            ('mahmut', 4)
        ])

    def test_items(self):
        l = [('armut', 2), ('elma', 1), ('kel', 3), ('mahmut', 4)]
        self.assertEqual(list(self.sd.items()), l)

    def test_keys(self):
        l = ['armut', 'elma', 'kel', 'mahmut']
        self.assertEqual(list(self.sd.keys()), l)

    def test_values(self):
        l = [2, 1, 3, 4]
        self.assertEqual(list(self.sd.values()), l)

    def test_iter(self):
        l = []
        for k in self.sd:
            l.append(k)
        self.assertEqual(l, ['armut', 'elma', 'kel', 'mahmut'])


if __name__ == '__main__':
    unittest.main()
