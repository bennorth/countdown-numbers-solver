from dataclasses import dataclass
from typing import List, Union


Node = Union["ValueNode", "OpNode"]


@dataclass
class ValueNode:
    value: int

    @classmethod
    def from_string(cls, s):
        v = int(s[2:-1])
        return cls(v)


@dataclass
class OpNode:
    children: List[Node]
    ops: str
