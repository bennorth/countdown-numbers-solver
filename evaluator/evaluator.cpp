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
