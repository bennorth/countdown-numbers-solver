from dataclasses import dataclass
from typing import List, Union


Node = Union["ValueNode", "OpNode"]


@dataclass
class ValueNode:
    value: int
    is_leaf = True

    @classmethod
    def from_string(cls, s):
        v = int(s[2:-1])
        return cls(v)

    def key(self):
        return (0, self.value)

    def in_canonical_order(self):
        return self

    def absorbing_like_children(self):
        return self


@dataclass
class OpNode:
    children: List[Node]
    ops: str
    is_leaf = False

    @classmethod
    def from_string(cls, s, stack):
        raw_ops = s[2:-1]
        ops = (raw_ops if s[0] == "A"
               else raw_ops.replace("+", "*").replace("-", "/"))
        n_operands = len(ops)
        operands = stack[-n_operands:]
        stack[-n_operands:] = []
        return cls(operands, ops)

    @property
    def is_addition(self):
        return self.ops[0] in "+-"

    def in_canonical_order(self):
        aug_children = list(zip(
            self.ops,
            [ch.in_canonical_order() for ch in self.children]
        ))

        # "+" happens to sort before "-" and "*" before "/".
        aug_children.sort(key=lambda t: (t[0], t[1].key()))

        canon_ops = "".join(t[0] for t in aug_children)
        canon_children = [t[1] for t in aug_children]

        return OpNode(canon_children, canon_ops)

    def key(self):
        k = 2 if self.is_addition else 1
        return (k,) + tuple(
            ((0 if op in "+*" else 1),) + ch.key()
            for ch, op in zip(self.children, self.ops))


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
