from dataclasses import dataclass
from enum import Enum
from typing import Optional, Union, List


TreeNode = Union["LeafNode", "InternalNode"]


@dataclass
class LeafNode:
    value: int

    def all_extended(self, value):
        yield InternalNode([self, LeafNode(value)])


class OpcodeKind(Enum):
    Value = 0
    MultiplyN = 1
    AddN = 2
    Return = 3

    @staticmethod
    def op_symbol(k):
        if k is None:
            return "?"
        elif k == OpcodeKind.MultiplyN:
            return "*"
        elif k == OpcodeKind.AddN:
            return "+"
        else:
            raise ValueError(f"no symbol for {k}")


def replace_element(xs, i, x):
    xs_copy = xs[:]
    xs_copy[i] = x
    return xs_copy


@dataclass
class InternalNode:
    children: List[TreeNode]
    op_kind: Optional[OpcodeKind] = None

    def all_extended(self, value):
        yield InternalNode([self, LeafNode(value)])
        yield InternalNode(self.children + [LeafNode(value)])
        for idx, child in enumerate(self.children):
            for t in child.all_extended(value):
                yield InternalNode(replace_element(self.children, idx, t))
