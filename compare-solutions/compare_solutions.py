from dataclasses import dataclass
from typing import List, Union
import subprocess


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
    def ops_inverted(self):
        inverted = {"+": "-", "-": "+", "*": "/", "/": "*"}
        return "".join(inverted[op] for op in self.ops)

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

    def absorbing_like_children(self):
        children = []
        ops = ""
        for orig_child, op in zip(self.children, self.ops):
            ch = orig_child.absorbing_like_children()
            if ch.is_leaf:
                children.append(ch)
                ops += op
            else:
                if self.is_addition == ch.is_addition:
                    children.extend(ch.children)
                    ops += (ch.ops if op in "+*" else ch.ops_inverted)
                else:
                    children.append(ch)
                    ops += op
        return OpNode(children, ops)

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


def solutions_from_cmd(cmd, target, cards):
    args = [cmd] + [str(n) for n in [target] + cards]
    cmd_result = subprocess.run(args, capture_output=True, encoding="utf-8")
    return list(
        (tree_from_string(line)
         .absorbing_like_children()
         .in_canonical_order()
         .key())
        for line in cmd_result.stdout.splitlines()
    )
