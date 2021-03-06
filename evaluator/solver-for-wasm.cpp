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
