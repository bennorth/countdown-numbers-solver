#!/bin/bash

PROGRAMS_BIN=programs-6-cards.bin
PROGRAMS_H=programs-6-cards.h
PROGRAMS_CPP=programs-6-cards.cpp

(
    cd ../tree-programs
    python compile_trees.py 6
) > $PROGRAMS_BIN

# Seems a bit odd to run xxd twice, but it's quick, and saves using
# and cleaning up a temporary file.

xxd -i $PROGRAMS_BIN \
    | head -n -1 \
    > $PROGRAMS_CPP

cat > $PROGRAMS_H <<EOF
#include <cstddef>
extern unsigned char programs_6_cards_bin[];
EOF

xxd -i $PROGRAMS_BIN \
    | tail -n 1 \
    | sed 's/unsigned int/static const size_t/' \
    >> $PROGRAMS_H

g++ -o test_evaluator -DEVALUATOR_PPRINT programs-6-cards.cpp evaluator.cpp evaluator_pprint.cpp test_evaluator.cpp
