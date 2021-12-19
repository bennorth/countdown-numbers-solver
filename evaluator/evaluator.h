#include <cstdint>
#include <cstddef>
#include <functional>
#include <boost/container/small_vector.hpp>

template<typename T> using small_vector = boost::container::small_vector<T, 12>;


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
const Opcode * all_programs();


template<OpcodeKind Kind> struct operator_traits {};

template<> struct operator_traits<OpcodeKind::MultiplyN> {
  static const int identity = 1;
  static void accumulate(int & x, int a) { x *= a; }
  static bool operand_is_valid(int x) { return x != 1; }
  static bool operation_is_valid(int noninv, int inv) { return (noninv % inv) == 0; }
  static int result(int noninv, int inv) { return noninv / inv; }
};

template<> struct operator_traits<OpcodeKind::AddN> {
  static const int identity = 0;
  static void accumulate(int & x, int a) { x += a; }
  static bool operand_is_valid(int x) { return true; }
  static bool operation_is_valid(int noninv, int inv) { return noninv > inv; }
  static int result(int noninv, int inv) { return noninv - inv; }
};


struct Evaluator {
  using output_function_t = std::function<void(const Evaluator &)>;

  small_vector<int> operands;
  const Opcode * instructions;
  const int * cards;
  const output_function_t & output;
  small_vector<Opcode> concrete_instructions;
};
