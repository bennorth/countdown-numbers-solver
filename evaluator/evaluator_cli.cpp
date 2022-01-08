#include <vector>
#include <string>
#include <iostream>

#include "evaluator.h"
#include "programs-6-cards.h"


int main(int argc, char ** argv)
{
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
