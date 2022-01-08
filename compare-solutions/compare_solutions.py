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

    def key(self):
        return (0, self.value)


@dataclass
class OpNode:
    children: List[Node]
    ops: str

    @classmethod
    def from_string(cls, s, stack):
        raw_ops = s[2:-1]
        ops = (raw_ops if s[0] == "A"
               else raw_ops.replace("+", "*").replace("-", "/"))
        n_operands = len(ops)
        operands = stack[-n_operands:]
        stack[-n_operands:] = []
        return cls(operands, ops)


def tree_from_string(s):
    opcodes = s.split()
    ops = []
    for c in opcodes:
        k = c[0]
        if k == "V":
            ops.append(ValueNode.from_string(c))
        elif k in "AM":
            result = OpNode.from_string(c, ops)
            ops.append(result)
        elif k == "R":
            # Should be last opcode.
            pass
    assert len(ops) == 1
    return ops[0]
