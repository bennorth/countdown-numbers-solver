from dataclasses import dataclass
from typing import List, Union
import subprocess
import random
from collections import Counter
import click


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

    def pprint_expr(self):
        return str(self.value)


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

    def pprint_expr(self):
        non_inv_ch = [
            ch.pprint_expr() for ch, op in zip(self.children, self.ops)
            if op in "+*"
        ]
        inv_ch = [
            ch.pprint_expr() for ch, op in zip(self.children, self.ops)
            if op not in "+*"
        ]
        non_inv_op = "+" if self.is_addition else "×"
        non_inv_part = f" {non_inv_op} ".join(non_inv_ch)
        inv_op = "−" if self.is_addition else "÷"
        inv_part = "" if not inv_ch else f" {inv_op} ".join([""] + inv_ch)
        return f"({non_inv_part}{inv_part})"

    def pprint_toplevel(self):
        # Strip top level of parentheses.
        return self.pprint_expr()[1:-1]


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


def all_solutions_from_cmd(cmd, target, cards):
    args = [cmd] + [str(n) for n in [target] + cards]
    cmd_result = subprocess.run(args, capture_output=True, encoding="utf-8")
    return list(
        tree_from_string(line)
        for line in cmd_result.stdout.splitlines()
    )


def compare_all_solutions(target, cards):
    return {
        "tree": all_solutions_from_cmd("./tree-solve", target, cards),
        "rpn": all_solutions_from_cmd("./rpn-solve", target, cards),
    }


def solutions_from_cmd(cmd, target, cards):
    return list(
        (tree
         .absorbing_like_children()
         .in_canonical_order()
         .key())
        for tree in all_solutions_from_cmd(cmd, target, cards)
    )


def compare_solvers(target, cards):
    tree_solutions = solutions_from_cmd("./tree-solve", target, cards)
    all_rpn_solutions = solutions_from_cmd("./rpn-solve", target, cards)

    rpn_solution_counts = Counter(all_rpn_solutions)

    comparison = {
        "target": target,
        "cards": cards,
        "tree": tree_solutions,
        "rpn": all_rpn_solutions,
        "tree_not_rpn": [],
        "rpn_not_tree": [],
    }

    for soln in tree_solutions:
        n_rpn = rpn_solution_counts[soln]
        if n_rpn == 0:
            comparison["tree_not_rpn"].append(soln)

    for soln in rpn_solution_counts:
        if soln not in tree_solutions:
            comparison["rpn_not_tree"].append(soln)

    return comparison


All_Cards = [25, 50, 75, 100] + list(range(1, 11))


@click.group()
def cli():
    pass


if __name__ == "__main__":
    n_done = 0
    while True:
        if n_done % 100 == 0:
            print(n_done)
        target = random.randint(100, 999)
        cards = random.choices(All_Cards, k=6)
        cmp = compare_solvers(target, cards)
        if cmp["tree_not_rpn"] or cmp["rpn_not_tree"]:
            print(cmp)
        n_done += 1
