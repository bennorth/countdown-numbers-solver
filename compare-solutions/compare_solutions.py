from dataclasses import dataclass


@dataclass
class ValueNode:
    value: int

    @classmethod
    def from_string(cls, s):
        v = int(s[2:-1])
        return cls(v)
