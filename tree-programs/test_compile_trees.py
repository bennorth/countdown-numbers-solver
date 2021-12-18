import pytest
import compile_trees as ct


class TestLeafNode:
    def test_creation(self):
        n = ct.LeafNode(23)
        assert n.value == 23

    def test_extension(self):
        V = ct.LeafNode
        I = ct.InternalNode
        n = V(23)
        assert list(n.all_extended(11)) == [I([V(23), V(11)])]


class TestHelpers:
    def test_replace_element(self):
        xs = [1, 5, 12, 10]
        ys = ct.replace_element(xs, 1, 66)
        assert xs == [1, 5, 12, 10]
        assert ys == [1, 66, 12, 10]
