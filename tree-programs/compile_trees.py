from dataclasses import dataclass
from enum import Enum
from typing import Optional, Union, List
from functools import reduce
from operator import concat
from itertools import combinations


TreeNode = Union["LeafNode", "InternalNode"]


class WithAsProgram:
    def as_program(self):
        return self.as_opcodes() + [Opcode(OpcodeKind.Return)]


@dataclass
class LeafNode(WithAsProgram):
    value: int

    def all_extended(self, value):
        yield InternalNode([self, LeafNode(value)])

    def with_opkind(self, k):
        return self

    def with_values(self, values):
        return LeafNode(values[self.value])

    def as_opcodes(self):
        return [Opcode(OpcodeKind.Value, self.value)]

    def as_sexp(self):
        return str(self.value)


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

    @staticmethod
    def other(k):
        if k == OpcodeKind.MultiplyN:
            return OpcodeKind.AddN
        elif k == OpcodeKind.AddN:
            return OpcodeKind.MultiplyN
        else:
            raise ValueError(f"no other for {k}")


def replace_element(xs, i, x):
    xs_copy = xs[:]
    xs_copy[i] = x
    return xs_copy


@dataclass
class InternalNode(WithAsProgram):
    children: List[TreeNode]
    op_kind: Optional[OpcodeKind] = None

    def all_extended(self, value):
        yield InternalNode([self, LeafNode(value)])
        yield InternalNode(self.children + [LeafNode(value)])
        for idx, child in enumerate(self.children):
            for t in child.all_extended(value):
                yield InternalNode(replace_element(self.children, idx, t))

    def with_opkind(self, k):
        children_with_other = [
            ch.with_opkind(OpcodeKind.other(k))
            for ch in self.children
        ]
        return InternalNode(children_with_other, k)

    def with_values(self, values):
        children_with_values = [
            ch.with_values(values)
            for ch in self.children
        ]
        return InternalNode(children_with_values, self.op_kind)

    def as_opcodes(self):
        children_opcodes = [ch.as_opcodes() for ch in self.children]
        own_opcodes = [Opcode(self.op_kind, len(self.children))]
        return reduce(concat, children_opcodes, []) + own_opcodes

    def as_sexp(self):
        sym = OpcodeKind.op_symbol(self.op_kind)
        children = " ".join(ch.as_sexp() for ch in self.children)
        return f"({sym} {children})"


def all_trees(n_leaves):
    if n_leaves == 1:
        yield LeafNode(0)
    else:
        for t in all_trees(n_leaves - 1):
            for t1 in t.all_extended(n_leaves - 1):
                yield t1


@dataclass
class Opcode:
    kind: OpcodeKind
    arg: Optional[int] = None

    def as_uint8(self):
        return (self.kind.value << 4) + (self.arg or 0)


def all_programs(n_cards):
    value_indexes = list(range(n_cards))

    program_u8s = []
    for n_used in range(1, n_cards + 1):
        for t in all_trees(n_used):
            t1 = t.with_opkind(OpcodeKind.AddN)
            for used_values in combinations(value_indexes, n_used):
                t2 = t1.with_values(used_values)
                for i in t2.as_program():
                    program_u8s.append(i.as_uint8())

    program_u8s.append(Opcode(OpcodeKind.Return).as_uint8())

    return program_u8s
