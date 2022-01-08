import compare_solutions

VN = compare_solutions.ValueNode


class TestValueNode:
    def test_from_string(self):
        n = VN.from_string("V(123)")
        assert n.value == 123
