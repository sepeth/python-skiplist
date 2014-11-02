import unittest
import sys
from skiplist import SortedSet


PY2 = sys.version_info[0] == 2


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

    @unittest.skipIf(PY2, "comparing different types is ok in PY2")
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


class TestSortedSetRepr(unittest.TestCase):
    def test_empty_sortedset_repr(self):
        s = SortedSet()
        self.assertEqual("SortedSet()", repr(s))

    def test_sortedset_repr_with_some_elements_in_it(self):
        s = SortedSet(['elma', 'armut', 'kel', 'mahmut'])
        self.assertEqual("SortedSet(['armut', 'elma', 'kel', 'mahmut'])",
                         repr(s))


class TestSortedSetContains(unittest.TestCase):
    def setUp(self):
        self.ss = SortedSet([7, 3, 12, 8])

    def test_in_operator_for_existing_element(self):
        self.assertIn(12, self.ss)

    def test_in_operator_for_nonexisting_element(self):
        self.assertNotIn(13, self.ss)


class TestSortedSetIssubset(unittest.TestCase):
    def test_longer_set_cannot_be_subset(self):
        longer = SortedSet([7, 3, 5])
        smaller = SortedSet([3, 5])
        self.assertFalse(longer.issubset(smaller))

    def test_set_is_subset_of_itself(self):
        s = SortedSet([5, 3, 7])
        self.assertTrue(s.issubset(s))

    def test_same_sets_should_be_subset_eachother(self):
        s1 = SortedSet([7, 3, 5])
        s2 = SortedSet([5, 7, 3])
        self.assertTrue(s1.issubset(s2))
        self.assertTrue(s2.issubset(s1))

    def test_smaller_set_scattered_throughout_longer_set(self):
        odds = SortedSet(range(1, 11, 2))
        numbers = SortedSet(range(0, 11))
        self.assertTrue(odds.issubset(numbers))

    def test_empty_set_issubset_of_everyset(self):
        e = SortedSet()
        s = SortedSet([7, 3, 5])
        self.assertTrue(e.issubset(s))

    def test_for_different_elements(self):
        s1 = SortedSet([3, 5, 7])
        s2 = SortedSet([3, 6, 7, 8])
        self.assertFalse(s1.issubset(s2))

    def test_issubset_with_a_list(self):
        s = SortedSet([3, 5, 7])
        l = [3, 5, 7, 9]
        self.assertTrue(s.issubset(l))


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
