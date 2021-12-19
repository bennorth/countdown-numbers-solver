#define CATCH_CONFIG_MAIN
#include "catch.hpp"

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
}
