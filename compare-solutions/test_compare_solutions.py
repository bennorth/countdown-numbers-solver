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

    def test_canonical_order(self):
        n = ON([ON([VN(100), VN(1)], "++"), VN(42), VN(400)], "*/*")
        assert n.in_canonical_order() == ON(
            [
                VN(400),
                ON([VN(1), VN(100)], "++"),
                VN(42)
            ],
            "**/"
        )

    def test_key(self):
        n = ON([ON([VN(100), VN(1)], "++"), VN(42), VN(400)], "*/*")
        assert n.key() == (
            1,    # multiplication
            (0,   # non-inverted ON([VN(100), VN(1)], "++")
             2,  # addition
             (0, 0, 100),  # non-inverted 100
             (0, 0, 1)),   # non-inverted 1
            (1, 0, 42),   # inverted VN(42)
            (0, 0, 400)   # non-inverted VN(400)
        )


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
