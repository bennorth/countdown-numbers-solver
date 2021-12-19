#include <cstdint>


enum class OpcodeKind : uint8_t {
  Value = 0,
  MultiplyN = 1,
  AddN = 2,
  Return = 3
};

inline OpcodeKind other_binop(OpcodeKind k) {
  switch (k) {
  case OpcodeKind::MultiplyN:
    return OpcodeKind::AddN;
  case OpcodeKind::AddN:
    return OpcodeKind::MultiplyN;
  default:
    return k;
  }
}
