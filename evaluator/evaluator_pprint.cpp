#include <cstddef>
#include <string>
#include <sstream>
#include "evaluator.h"

static void emit_binop_inversions_(
  std::ostringstream & oss,
  char label,
  Opcode opcode
) {
  oss << label << "(";
  auto n_args = static_cast<size_t>(opcode.arg0);
  auto non_inv_mask = static_cast<unsigned>(opcode.arg1);
  for (unsigned mask = 1 << (n_args - 1); mask != 0; mask >>= 1)
    oss << (((non_inv_mask & mask) != 0) ? "+" : "-");
  oss << ")";
}
