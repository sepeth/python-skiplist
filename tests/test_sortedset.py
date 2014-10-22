import unittest
from skiplist import SortedSet


class TestSortedSet(unittest.TestCase):
    def test_emply_len(self):
        self.assertEqual(len(SortedSet()), 0)

    def test_one_element(self):
        self.assertEqual(len(SortedSet([7])), 1)


class TestSortedSetAdd(unittest.TestCase):
    def setUp(self):
        self.ss = SortedSet()

    def test_add(self):
        self.ss.add(3)
        self.assertEqual(len(self.ss), 1)

    def test_add_with_an_elem_already_in_the_set(self):
        self.ss.add(3)
        self.ss.add(3)
        self.assertEqual(len(self.ss), 1)

    def test_raise_unorderable_types(self):
        self.ss.add(3)
        self.assertRaises(TypeError, self.ss.add, 'sepeth')


class TestSortedSetRemove(unittest.TestCase):
    def setUp(self):
        self.ss = SortedSet([7, 3, 12, 8])

    def test_remove_an_existing_elem(self):
        oldlen = len(self.ss)
        self.ss.remove(7)
        self.assertEqual(len(self.ss), oldlen - 1)

    def test_remove_an_elem_that_is_not_in_the_set(self):
        self.assertRaises(KeyError, self.ss.remove, 53153)

    def test_raise_unorderable_types(self):
        self.assertRaises(TypeError, self.ss.remove, 'sepeth')


class TestSortedSetSubscript(unittest.TestCase):
    def test_search_an_element_in_the_empty_set(self):
        ss = SortedSet()
        self.assertRaises(KeyError, ss.__getitem__, 7)


class TestSortedSetRepr(unittest.TestCase):
    def test_empty_sortedset_repr(self):
        s = SortedSet()
        self.assertEqual("SortedSet()", repr(s))

    def test_sortedset_repr_with_some_elements_in_it(self):
        s = SortedSet(['elma', 'armut', 'kel', 'mahmut'])
        self.assertEqual("SortedSet({'armut', 'elma', 'kel', 'mahmut'})",
                         repr(s))


class TestSortedSetContains(unittest.TestCase):
    def setUp(self):
        self.ss = SortedSet([7, 3, 12, 8])

    def test_in_operator_for_existing_element(self):
        self.assertTrue(12 in self.ss)

    def test_in_operator_for_nonexisting_element(self):
        self.assertFalse(13 in self.ss)


class TestSortedSetIter(unittest.TestCase):
    def test_empty_sortedset(self):
        l = []
        s = SortedSet()
        for item in s:
            l.append(item)
        for item in s:
            l.append(item)
        self.assertEqual(len(l), 0)

    def test_a_few_elements(self):
        s = SortedSet([7, 3, 5])
        l = []
        for item in s:
            l.append(item)
        self.assertEqual(l, [3, 5, 7])

    def test_creating_two_iter_from_the_same_sortedset(self):
        s = SortedSet([7, 3, 5])
        l = []
        for item in s:
            l.append(item)
        for item in s:
            l.append(item)
        self.assertEqual(l, [3, 5, 7, 3, 5, 7])


if __name__ == '__main__':
    unittest.main()
