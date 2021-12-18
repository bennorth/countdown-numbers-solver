import pytest
import compile_trees as ct


class TestLeafNode:
    def test_creation(self):
        n = ct.LeafNode(23)
        assert n.value == 23

    def test_extension(self):
        V = ct.LeafNode
        I = ct.InternalNode
        n = V(23)
        assert list(n.all_extended(11)) == [I([V(23), V(11)])]

    def test_as_sexp(self):
        assert ct.LeafNode(42).as_sexp() == "42"


class TestInternalNode:
    def test_extension(self):
        V = ct.LeafNode
        I = ct.InternalNode
        i = I([V(10), I([V(5), V(3)])])  # (? 10 (? 5 3))
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

    def test_as_sexp(self):
        V = ct.LeafNode
        I = ct.InternalNode
        i = I([V(10), I([V(5), V(3)])])
        assert i.as_sexp() == "(? 10 (? 5 3))"


class TestHelpers:
    def test_replace_element(self):
        xs = [1, 5, 12, 10]
        ys = ct.replace_element(xs, 1, 66)
        assert xs == [1, 5, 12, 10]
        assert ys == [1, 66, 12, 10]

    def test_op_symbol(self):
        K = ct.OpcodeKind
        assert K.op_symbol(None) == "?"
        assert K.op_symbol(K.MultiplyN) == "*"
        assert K.op_symbol(K.AddN) == "+"
        with pytest.raises(ValueError):
            K.op_symbol(K.Value)
        with pytest.raises(ValueError):
            K.op_symbol(K.Return)

    def test_op_other(self):
        K = ct.OpcodeKind
        assert K.other(K.MultiplyN) == K.AddN
        assert K.other(K.AddN) == K.MultiplyN
        with pytest.raises(ValueError):
            K.other(None)
        with pytest.raises(ValueError):
            K.other(K.Value)
        with pytest.raises(ValueError):
            K.other(K.Return)


class TestTopLevel:
    def test_all_trees(self):
        # True values taken from
        # Counting dendrograms: a survey, Fionn Murtagh
        # Discrete Applied Mathematics 7 (1984) 191-199
        n_trees = [len(list(ct.all_trees(n))) for n in range(1, 8)]
        assert n_trees == [1, 1, 4, 26, 236, 2752, 39208]
