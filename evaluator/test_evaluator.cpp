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
}
