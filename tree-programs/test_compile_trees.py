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
