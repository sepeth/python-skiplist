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


class TestSortedSetRemove(unittest.TestCase):
    def setUp(self):
        self.ss = SortedSet([7, 3, 12, 8])

    def test_remove_an_existing_elem(self):
        oldlen = len(self.ss)
        self.ss.remove(7)
        self.assertEqual(len(self.ss), oldlen - 1)

    def test_remove_an_elem_that_is_not_in_the_set(self):
        self.assertRaises(KeyError, self.ss.remove, 53153)


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
