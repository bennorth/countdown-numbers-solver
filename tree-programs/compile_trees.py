from dataclasses import dataclass
from enum import Enum
from typing import Union


TreeNode = Union["LeafNode", "InternalNode"]


@dataclass
class LeafNode:
    value: int


class OpcodeKind(Enum):
    Value = 0
    MultiplyN = 1
    AddN = 2
    Return = 3
