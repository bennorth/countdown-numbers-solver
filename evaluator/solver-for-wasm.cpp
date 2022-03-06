#include <array>
#include <vector>
#include <stdint.h>
#include "evaluator.h"

#include <emscripten/bind.h>

struct CountdownSolver {
  std::array<int, 6> cards;
  int target;
  std::vector<Opcode> solutions;

  CountdownSolver(int c1, int c2, int c3, int c4, int c5, int c6, int target)
    : cards{c1, c2, c3, c4, c5, c6}
    , target(target)
  {}

  void solve() {
    Evaluator::output_function_t process_candidate{
      [this](const Evaluator & e) {
        if (e.value() == target)
          solutions.insert(solutions.end(),
                           e.concrete_instructions.begin(),
                           e.concrete_instructions.end());
      }};

    const Opcode * program{all_programs()};
    Evaluator e(program, cards.data(), process_candidate);

    bool more_follow = true;
    while (more_follow)
      more_follow = e.all_valid();
  }

  const intptr_t solution_programs() const {
    return reinterpret_cast<intptr_t>(solutions.data());
  }

  const int solution_programs_nbytes() const {
    return solutions.size() * sizeof(Opcode);
  }
};


EMSCRIPTEN_BINDINGS(countdown_solver) {
  emscripten::class_<CountdownSolver>("CountdownSolver")
    .constructor<int, int, int, int, int, int, int>()
    .function("solve", &CountdownSolver::solve)
    .function("solutionPrograms", &CountdownSolver::solution_programs)
    .function("solutionProgramsNBytes", &CountdownSolver::solution_programs_nbytes)
    ;
}
