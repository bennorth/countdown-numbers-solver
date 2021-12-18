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


def replace_element(xs, i, x):
    xs_copy = xs[:]
    xs_copy[i] = x
    return xs_copy


@dataclass
class InternalNode:
    children: List[TreeNode]
    op_kind: Optional[OpcodeKind] = None
