import compare_solutions

VN = compare_solutions.ValueNode
ON = compare_solutions.OpNode


class TestValueNode:
    def test_from_string(self):
        n = VN.from_string("V(123)")
        assert n.value == 123

    def test_key(self):
        v = VN(42)
        assert v.key() == (0, 42)


class TestOpNode:
    def test_from_string_add(self):
        ops = [VN(23), VN(100), VN(42), VN(2)]
        n = ON.from_string("A(+-+)", ops)
        assert ops == [VN(23)]
        assert n.children == [VN(100), VN(42), VN(2)]
        assert n.ops == "+-+"

    def test_from_string_mult(self):
        ops = [VN(23), VN(100), VN(42), VN(2)]
        n = ON.from_string("M(++-)", ops)
        assert ops == [VN(23)]
        assert n.children == [VN(100), VN(42), VN(2)]
        assert n.ops == "**/"


class TestTopLevel:
    def test_tree_from_string(self):
        got_n = compare_solutions.tree_from_string(
            "V(24) V(13) A(+-) V(99) M(-+) R"
        )
        exp_n = ON(
            [
                ON([VN(24), VN(13)], "+-"),
                VN(99)
            ],
            "/*"
        )
        assert got_n == exp_n
