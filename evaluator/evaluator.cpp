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
}
