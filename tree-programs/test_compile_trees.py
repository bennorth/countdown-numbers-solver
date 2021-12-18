import pytest
import compile_trees as ct


class TestLeafNode:
    def test_creation(self):
        n = ct.LeafNode(23)
        assert n.value == 23
