from dataclasses import dataclass
from enum import Enum
from typing import Optional, Union, List


TreeNode = Union["LeafNode", "InternalNode"]


@dataclass
class LeafNode:
    value: int


class OpcodeKind(Enum):
    Value = 0
    MultiplyN = 1
    AddN = 2
    Return = 3


@dataclass
class InternalNode:
    children: List[TreeNode]
    op_kind: Optional[OpcodeKind] = None
