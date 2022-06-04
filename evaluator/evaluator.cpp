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

#include <array>
#include <cstddef>
#include "programs-6-cards.h"
#include "evaluator.h"

static const uint8_t * programs_6_cards_bin_end
  = programs_6_cards_bin + programs_6_cards_bin_len;

static std::array<Opcode, programs_6_cards_bin_len * 2> all_programs_;

const uint8_t * all_packed_programs()
{
  return programs_6_cards_bin;
}

size_t n_packed_opcodes()
{
  return programs_6_cards_bin_len;
}

const Opcode * all_programs()
{
  static Opcode * programs{([]()
    {
      Opcode * opcode = &(all_programs_[0]);

      for (
        const uint8_t * packed_opcode = programs_6_cards_bin;
        packed_opcode != programs_6_cards_bin_end;
        ++packed_opcode
      ) {
        *opcode++ = {
          static_cast<OpcodeKind>(*packed_opcode >> 4),
          static_cast<uint8_t>(*packed_opcode & 0x0f),
          0
        };
      }

      // Prepare to overwrite second Return:
      --opcode;

      for (
        const uint8_t * packed_opcode = programs_6_cards_bin;
        packed_opcode != programs_6_cards_bin_end;
        ++packed_opcode
      ) {
        *opcode++ = {
          other_binop(static_cast<OpcodeKind>(*packed_opcode >> 4)),
          static_cast<uint8_t>(*packed_opcode & 0x0f),
          0
        };
      }

      return &(all_programs_[0]);
    })()};

  return programs;
}


////////////////////////////////////////////////////////////////////////

bool Evaluator::all_valid()
{
  auto program_finished{false};
  while (!program_finished)
  {
    Opcode instruction{*instructions++};

    switch (instruction.kind)
    {
    case OpcodeKind::Value:
      operands.push_back(cards[instruction.arg0]);
      concrete_instructions.push_back(instruction);
      break;

    case OpcodeKind::MultiplyN:
    case OpcodeKind::AddN:
      for (
        unsigned non_inv_mask = 1;
        non_inv_mask != (1U << instruction.arg0);
        ++non_inv_mask
      ) {
        Evaluator e{*this};
        e.all_valid(instruction, non_inv_mask);
      }

      // Skip rest of program; individual "child" Evaluators will have
      // processed it.
      while (instructions++->kind != OpcodeKind::Return)
        ;

      program_finished = true;
      break;

    case OpcodeKind::Return:
      concrete_instructions.push_back(instruction);
      output(*this);
      program_finished = true;
      break;

    default:
      return false;
    }
  }

  // Get ready for next program (if there is one).
  clear();

  // Return value indicates whether there's a double-Return.
  return (instructions->kind != OpcodeKind::Return);
}

void Evaluator::all_valid(Opcode instruction, unsigned non_inv_mask) {
  switch (instruction.kind)
  {
  case OpcodeKind::MultiplyN:
    all_valid<OpcodeKind::MultiplyN>(instruction.arg0, non_inv_mask);
    break;

  case OpcodeKind::AddN:
    all_valid<OpcodeKind::AddN>(instruction.arg0, non_inv_mask);
    break;

  default:
    // Error.
    break;
  }
}

bool Evaluator::is_clear() const
{
  return (
    (operands.size() == 0)
    && (concrete_instructions.size() == 0)
  );
}
