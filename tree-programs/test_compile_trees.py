from dataclasses import dataclass
from typing import Optional
import pytest
import compile_trees as ct


L = ct.LeafNode
I = ct.InternalNode
K = ct.OpcodeKind
C = ct.Opcode


class TestLeafNode:
    def test_creation(self):
        n = L(23)
        assert n.value == 23

    def test_extension(self):
        n = L(23)
        assert list(n.all_extended(11)) == [I([L(23), L(11)])]

    def test_with_opkind(self):
        n = L(23)
        assert n.with_opkind(K.MultiplyN) == n

    def test_with_values(self):
        n = L(2)
        assert n.with_values([42, 99, 101, 33]) == L(101)

    def test_as_opcodes(self):
        n = L(42)
        assert n.as_opcodes() == [C(K.Value, 42)]

    def test_as_sexp(self):
        assert L(42).as_sexp() == "42"


class TestInternalNode:
    def test_extension(self):
        i = I([L(10), I([L(5), L(3)])])  # (? 10 (? 5 3))
        exp_extensions_sexps = [
            "(? (? 10 (? 5 3)) 11)",
            "(? 10 (? 5 3) 11)",
            "(? (? 10 11) (? 5 3))",
            "(? 10 (? (? 5 3) 11))",
            "(? 10 (? 5 3 11))",
            "(? 10 (? (? 5 11) 3))",
            "(? 10 (? 5 (? 3 11)))",
        ]
        got_extension_sexps = [e.as_sexp() for e in i.all_extended(11)]
        assert sorted(got_extension_sexps) == sorted(exp_extensions_sexps)

    def test_with_opkind(self):
        i = I([L(10), I([L(5), L(3)])])  # (? 10 (? 5 3))
        i1 = i.with_opkind(K.MultiplyN)
        assert i1.as_sexp() == "(* 10 (+ 5 3))"

    def test_with_values(self):
        i = I([L(1), I([L(0), L(2)])]).with_opkind(K.MultiplyN)
        i1 = i.with_values([99, 42, 101])
        assert i1.as_sexp() == "(* 42 (+ 99 101))"

    def test_as_opcodes(self):
        i = I([L(10), I([L(4), L(5), L(3)], K.MultiplyN)], K.AddN)
        assert i.as_opcodes() == [
            C(K.Value, 10),
            C(K.Value, 4),
            C(K.Value, 5),
            C(K.Value, 3),
            C(K.MultiplyN, 3),
            C(K.AddN, 2),
        ]

    def test_as_sexp(self):
        i = I([L(10), I([L(5), L(3)])])
        assert i.as_sexp() == "(? 10 (? 5 3))"


class TestHelpers:
    def test_replace_element(self):
        xs = [1, 5, 12, 10]
        ys = ct.replace_element(xs, 1, 66)
        assert xs == [1, 5, 12, 10]
        assert ys == [1, 66, 12, 10]

    def test_op_symbol(self):
        assert K.op_symbol(None) == "?"
        assert K.op_symbol(K.MultiplyN) == "*"
        assert K.op_symbol(K.AddN) == "+"
        with pytest.raises(ValueError):
            K.op_symbol(K.Value)
        with pytest.raises(ValueError):
            K.op_symbol(K.Return)

    def test_op_other(self):
        assert K.other(K.MultiplyN) == K.AddN
        assert K.other(K.AddN) == K.MultiplyN
        with pytest.raises(ValueError):
            K.other(None)
        with pytest.raises(ValueError):
            K.other(K.Value)
        with pytest.raises(ValueError):
            K.other(K.Return)


@dataclass
class EncodingTestSpec:
    kind: K
    arg: Optional[int]
    exp_u8: int

    @property
    def label(self):
        tag = self.kind.name[0]
        sfx = str(self.arg) if self.arg else ""
        return tag + sfx


class TestOpcode:
    @pytest.mark.parametrize(
        "spec",
        [
            EncodingTestSpec(K.MultiplyN, 3, 0x13),
            EncodingTestSpec(K.AddN, 4, 0x24),
            EncodingTestSpec(K.Value, 5, 0x05),
            EncodingTestSpec(K.Return, None, 0x30),
        ],
        ids=(lambda s: s.label)
    )
    def test_encoding(self, spec):
        c = C(spec.kind, spec.arg)
        assert c.as_uint8() == spec.exp_u8


class TestTopLevel:
    def test_all_trees(self):
        # True values taken from
        # Counting dendrograms: a survey, Fionn Murtagh
        # Discrete Applied Mathematics 7 (1984) 191-199
        n_trees = [len(list(ct.all_trees(n))) for n in range(1, 8)]
        assert n_trees == [1, 1, 4, 26, 236, 2752, 39208]
