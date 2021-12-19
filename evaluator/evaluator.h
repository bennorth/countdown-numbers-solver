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

  Evaluator(const Opcode * instructions, const int * cards, const output_function_t & output)
    : instructions(instructions)
    , cards(cards)
    , output(output)
  {}

  Evaluator(const Evaluator & rhs)
    : operands(rhs.operands)
    , instructions(rhs.instructions)
    , cards(rhs.cards)
    , output(rhs.output)
    , concrete_instructions(rhs.concrete_instructions)
  {}

  void clear()
  {
    operands.clear();
    concrete_instructions.clear();
  }

  // Return value is "do more programs follow the one just executed"?
  bool all_valid();

  template<OpcodeKind OpKind>
  void all_valid(uint8_t n_args, unsigned non_inv_mask);

  int value() const { return operands.back(); }
};


template<OpcodeKind Kind>
void Evaluator::all_valid(uint8_t n_args, unsigned non_inv_mask)
{
  using op_traits = operator_traits<Kind>;
  int inverting_input{op_traits::identity};
  int non_inverting_input{op_traits::identity};

  unsigned mask{1};
  for (int i = 0; i != n_args; ++i, mask <<= 1) {
    const int value = operands.back();
    operands.pop_back();

    if (!op_traits::operand_is_valid(value))
      return;

    int & accumulator{
      (non_inv_mask & mask) ? non_inverting_input : inverting_input
    };

    op_traits::accumulate(accumulator, value);
  }

  if (op_traits::operation_is_valid(non_inverting_input, inverting_input))
  {
    operands.push_back(
      op_traits::result(non_inverting_input, inverting_input)
    );

    concrete_instructions.push_back(
      { Kind, n_args, static_cast<uint8_t>(non_inv_mask) }
    );

    all_valid();
  }
}
