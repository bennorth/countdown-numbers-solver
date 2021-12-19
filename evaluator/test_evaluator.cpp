#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <array>
#include "evaluator.h"

TEST_CASE("OpcodeKind manipulation", "")
{
  REQUIRE(other_binop(OpcodeKind::Value) == OpcodeKind::Value);
  REQUIRE(other_binop(OpcodeKind::MultiplyN) == OpcodeKind::AddN);
  REQUIRE(other_binop(OpcodeKind::AddN) == OpcodeKind::MultiplyN);
  REQUIRE(other_binop(OpcodeKind::Return) == OpcodeKind::Return);
}

TEST_CASE("Program expansion", "")
{
  SECTION("Expand 0x23")
  {
    const uint8_t * packed = all_packed_programs();

    // Look for an interesting one.
    const uint8_t * packed_0x23 = packed;
    size_t idx = 0;
    for (; *packed_0x23 != 0x23; ++packed_0x23, ++idx)
      ;

    const Opcode * opcode = all_programs() + idx;
    REQUIRE(opcode->kind == OpcodeKind::AddN);
    REQUIRE(opcode->arg0 == 3);
  }

  SECTION("All arg1 zero")
  {
    int n_returns = 0;
    int n_non_zero_arg1 = 0;
    const Opcode * opcode = all_programs();

    while (n_returns != 2) {
      if (opcode->arg1 != 0)
        ++n_non_zero_arg1;

      if (opcode->kind == OpcodeKind::Return)
        ++n_returns;
      else
        n_returns = 0;

      ++opcode;
    }

    REQUIRE(n_non_zero_arg1 == 0);

    size_t n_expanded_opcodes = opcode - all_programs();
    REQUIRE(n_expanded_opcodes == n_packed_opcodes() * 2 - 1);
  }
}

TEST_CASE("Operator traits", "")
{
  SECTION("Multiplication")
  {
    using multiply_traits = operator_traits<OpcodeKind::MultiplyN>;
    REQUIRE(multiply_traits::operand_is_valid(42));
    REQUIRE( ! multiply_traits::operand_is_valid(1));
    REQUIRE(multiply_traits::operation_is_valid(60, 5));
    REQUIRE( ! multiply_traits::operation_is_valid(60, 7));
    REQUIRE(multiply_traits::result(60, 5) == 12);

    int x = 42;
    multiply_traits::accumulate(x, 10);
    REQUIRE(x == 420);
  }

  SECTION("Add")
  {
    using add_traits = operator_traits<OpcodeKind::AddN>;
    REQUIRE(add_traits::operand_is_valid(42));
    REQUIRE(add_traits::operand_is_valid(1));
    REQUIRE(add_traits::operation_is_valid(60, 5));
    REQUIRE( ! add_traits::operation_is_valid(60, 67));
    REQUIRE(add_traits::result(60, 5) == 55);

    int x = 42;
    add_traits::accumulate(x, 10);
    REQUIRE(x == 52);
  }
}

TEST_CASE("Operator-free programs", "")
{
  std::vector<Opcode> programs{
    { OpcodeKind::Value, 0, 0 },
    { OpcodeKind::Return, 0, 0 },
    { OpcodeKind::Value, 2, 0 },
    { OpcodeKind::Return, 0, 0 },
    { OpcodeKind::Return, 0, 0 },
  };

  std::vector<int> cards{42, 33, 99, 100, 101, 12};
  int value{};
  Evaluator::output_function_t store_value{
    [&value](const Evaluator & e) { value = e.value(); }
  };

  Evaluator evaluator{programs.data(), cards.data(), store_value};

  const auto more_follow_1 = evaluator.all_valid();
  REQUIRE(more_follow_1);
  REQUIRE(value == 42);
  REQUIRE(evaluator.is_clear());

  const auto more_follow_2 = evaluator.all_valid();
  REQUIRE( ! more_follow_2);
  REQUIRE(value == 99);
  REQUIRE(evaluator.is_clear());
}
