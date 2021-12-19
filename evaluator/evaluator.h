#include <cstdint>
#include <cstddef>


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

struct __attribute__ ((packed)) Opcode {
  OpcodeKind kind;
  uint8_t arg0;  // Card-index for Value; n.operands for MultiplyN/AddN
  uint8_t arg1;  // Non-inverted input mask for MultiplyN/AddN
};

const uint8_t * all_packed_programs();
size_t n_packed_opcodes();
