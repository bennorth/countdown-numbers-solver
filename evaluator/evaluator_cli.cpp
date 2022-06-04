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

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

#include "evaluator.h"
#include "programs-6-cards.h"


int main(int argc, char ** argv)
{
  if (argc < 2) {
    std::cerr << "Usage: evaluator-cli TARGET CARD_1 CARD_2 ... CARD_N\n";
    return EXIT_FAILURE;
  }

  std::vector<int> cards;

  for (int i = 1; i != argc; ++i) {
    std::string arg{argv[i]};
    cards.push_back(std::stoi(arg));
  }

  // The first 'card' is actually the target.
  int target{cards.front()};
  cards.erase(cards.begin());

  Evaluator::output_function_t emit_if_match{
    [target](const Evaluator & e)
    {
      if (e.value() == target)
        std::cout << e.pprint_concrete_flat() << "\n";
    }
  };

  Evaluator evaluator{all_programs(), cards.data(), emit_if_match};

  while (true)
  {
    const auto more_follow = evaluator.all_valid();
    if (!more_follow)
      break;
  }

  return 0;
}
