/*
  Copyright 2022 Ben North

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see https://www.gnu.org/licenses/
*/

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

TEST_CASE("Generate add/multiply inversions", "")
{
  std::vector<Opcode> programs{
    { OpcodeKind::Value, 0, 0 },
    { OpcodeKind::Value, 1, 0 },
    { OpcodeKind::Value, 2, 0 },
    { OpcodeKind::Value, 3, 0 },
    { OpcodeKind::AddN, 4, 0 },
    { OpcodeKind::Value, 4, 0 },
    { OpcodeKind::MultiplyN, 2, 0 },
    { OpcodeKind::Return, 0, 0 },
    { OpcodeKind::Return, 0, 0 },
  };

  std::vector<int> cards{1, 2, 4, 8, 3, 200};

  std::vector<int> values{};

  Evaluator::output_function_t gather_value{
    [&values](const Evaluator & e)
    {
      values.push_back(e.value());
    }
  };

  Evaluator evaluator{programs.data(), cards.data(), gather_value};

  const auto more_follow = evaluator.all_valid();
  REQUIRE( ! more_follow);
  REQUIRE(evaluator.is_clear());

  // Stack will be
  //
  //     1 2 4 8  (with 8 on top)
  //
  // Inversions tried will be, in order,
  //
  //     - - - +  -1 -2 -4  8  =  1
  //     - - + -  -1 -2  4 -8
  //     - - + +  -1 -2  4  8  =  9
  //     - + - -  -1  2 -4 -8
  //     - + - +  -1  2 -4  8  =  5
  //     - + + -  -1  2  4 -8
  //     - + + +  -1  2  4  8  = 13
  //     + - - -   1 -2 -4 -8
  //     + - - +   1 -2 -4  8  =  3
  //     + - + -   1 -2  4 -8
  //     + - + +   1 -2  4  8  = 11
  //     + + - -   1  2 -4 -8
  //     + + - +   1  2 -4  8  =  7
  //     + + + -   1  2  4 -8
  //     + + + +   1  2  4  8  = 15
  //
  // Although 1 is a valid result from the AddN operation, the
  // MultiplyN will reject it because multiply-by-1 is forbidden.
  // We'll try multiplying each of the above by 3, dividing it by 3,
  // and dividing 3 by it.  E.g., for 15:
  //
  //     / *  /15  3
  //     * /   15 /3  =  5
  //     * *   15  3  = 45
  //
  std::vector<int> expected_values{
    3, 27, 15, 39, 1, 1, 9, 33, 21, 5, 45
  };

  REQUIRE(values == expected_values);
}

TEST_CASE("Pretty-printing", "")
{
  std::vector<int> cards{1, 2, 4, 8, 3, 200};
  std::vector<Opcode> program{
    { OpcodeKind::Value, 0, 0 },
    { OpcodeKind::Value, 1, 0 },
    { OpcodeKind::Value, 2, 0 },
    { OpcodeKind::Value, 3, 0 },
    { OpcodeKind::AddN, 4, 12 },
    { OpcodeKind::Value, 4, 0 },
    { OpcodeKind::MultiplyN, 2, 1 },
    { OpcodeKind::Return, 0, 0 }
  };

  const auto pprinted{pprint_opcodes(program, cards.data())};

  const std::vector<std::string> expected_pprinted{
    "V(1)", "V(2)", "V(4)", "V(8)", "A(++--)", "V(3)", "M(-+)", "R"
  };

  REQUIRE(pprinted == expected_pprinted);
}
