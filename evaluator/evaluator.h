#include <cstdint>


enum class OpcodeKind : uint8_t {
  Value = 0,
  MultiplyN = 1,
  AddN = 2,
  Return = 3
};
