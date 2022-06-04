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

#include <cstddef>
#include <vector>
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

std::string pprint_opcode(Opcode opcode, const int * cards)
{
  std::ostringstream oss;

  switch (opcode.kind)
  {
  case OpcodeKind::Value:
    oss << "V(" << cards[opcode.arg0] << ")";
    break;
  case OpcodeKind::MultiplyN:
    emit_binop_inversions_(oss, 'M', opcode);
    break;
  case OpcodeKind::AddN:
    emit_binop_inversions_(oss, 'A', opcode);
    break;
  case OpcodeKind::Return:
    oss << "R";
    break;
  default:
    oss << "X";
    break;
  }

  return oss.str();
}

std::vector<std::string> Evaluator::pprint_concrete() const
{
  return pprint_opcodes(concrete_instructions, cards);
}

std::string Evaluator::pprint_concrete_flat() const
{
  std::ostringstream oss;
  auto opcodes{pprint_concrete()};
  std::copy(
    opcodes.begin(),
    opcodes.end(),
    std::ostream_iterator<std::string>(oss, " ")
  );

  // Trim off trailing space:
  const auto str = oss.str();
  return str.substr(0, str.size() - 1);
}
