# Finding solutions to the Countdown numbers round

## General context

On the TV gameshow Countdown, there’s a round where six number cards
are chosen, along with a target number.  Contestants have to use a
subset of the six numbers to construct an arithmetic expression which
evaluates to the target number, or as close as they can get to it.
They’re only allowed the four basic operations (addition, subtraction,
multiplication and division), and at all stages of evaluation, the
result so far must be positive and integer.

## Context of interest

There was some recent interest in this game in our house, and, while
realising that I was not going to be the first person to think about
this, I wanted to explore how one could write a computer solver.  One
existing web page which ranks highly in web search results is
(http://happysoft.org.uk/countdown/numgame.php).

## My approach

The approach I took was to try to construct a parse tree for the
expression.  Initially I thought a binary tree would be a good place
to start, followed by a refinement pass to a n-ary tree, to collapse,
e.g., “(3 + 4) + 5” into “3 + 4 + 5”.  Addition and multiplication are
both commutative and associative, so this would be legitimate, and,
further, the ordering of an operation node’s children is unimportant.
I was hoping there would then be some way to throw out duplicate
trees.

After a bit more thought, though, it seemed cleaner to start directly
with the generation of all possible n-ary trees.  Some care is needed
to be precise about what this means — THIS PAPER, addressing
dendrograms, gives a useful classification of the different ways of
thinking about “all trees”.  It talks about whether the leaf nodes are
“labelled” or “unlabelled”, and whether the order of a node’s children
is important.  It also distinguishes between “ranked” and “unranked”
trees.  CHECK.  We want “labelled”, “unordered”, “unranked” trees.
CHECK.

The operation nodes have to alternate as you travel from the root to
the leaves, between “+” and “×”.

EXAMPLE picture and interpretation.

Subtraction and division are handled by tagging each non-root node to
say whether its value is used by its parent in a non-inverted or
inverted sense.  An inverted child of a “+” node is actually
subtracted, and an inverted child of a “×” node is actually divided
by.  It is not permitted for all children of a node to be inverted.

EXAMPLE picture and interpretation including subtraction and division.

The question then is how to generate all n-ary trees with up to 6 leaf
nodes.  One recursive solution to this problem is described in THAT
PAPER.  Labelling the leaf nodes from zero up to (N-1), there is only
one tree with one node:

PICTURE.

(Could we start recursion with zero leaf nodes?  Seems a bit
artificial.)

To take the set of m-leaf-node trees and generate the set of
(m+1)-leaf-node trees, we proceed as follows.  We must decide where to
put the node with label “m”.  For each m-leaf-node tree, we consider
inserting the new node “at” that node.  What this means depends on
whether the chosen node of the small tree is a branch node or a leaf
node.

To insert a new node “at a branch node” means to add the new node as
an additional child of that node.

PICTURE

To insert a new node “at a leaf node” means to replace that leaf node
with a branch node having two children: the original leaf node and the
new node.

The paper gives the total number of such trees in its SECTION
BLAHBLAH.

Given this large set of trees, we can generate all possible Countdown
expressions with a few extra steps.  For example, suppose we have a
4-leaf-node tree.

We can choose the way the four leaf node labels correspond to four of
the six number cards in (6 4)=15 ways.

For each such choice, we can either assign the root node to be a “+”
node or a “×” node.

For each such choice, we alternate between “+” and “×” as we follow
each path to leaf nodes.  With that done, we look at each operation
(branch) node in turn.  For each one, we list all proper subsets of
the set of that node’s children.  Then we loop over the Cartesian
product of choices of subsets for all operation nodes.

For each operation node and its chosen subset of children, we mark
that subset of that node’s children as “inverted” and the rest of its
children as “non-inverted”.  The requirement that we choose a proper
subset means we satisfy the rule that we can’t have an operation node
all of whose children are inverted.

Finally we can evaluate the expression and test whether the result
matches the target.

Implementation: For the implementation, I split the task after the
point of generating the k-node n-ary trees and choosing which k cards
from 6 they applied to, and dumped representations of those trees as
‘programs’ for a stack-based machine.  The instructions this machine
understands are “push value onto the stack” and “pop the top N values
off the stack, combine them with OPERATION, and push the result back
on the stack”.  The OPERATION can be + or ×.  The exploration of the
choices of which inputs to invert is deferred until evaluation time.

EXAMPLE of tree and its matching program.

That part was done in Python, and resulted in NNNNN programs.  These
programs are hard-coded into a separate C++ program, which interprets
them as program templates.  It pre-processes the list by copying and
then swapping “+”/“×” in the copies.  It executes the double-size list
of programs by following all the different execution paths arising
from the different subset choices.  To ensure we keep to the rules
about always having positive integer intermediate results, it
processes all non-inverted arguments before the inverted ones.  If the
result fails to be a positive integer, we abandon that execution
branch.  We also abandon a branch if it involves multiplying by 1.
This is not strictly against the rules, but it seems cleaner to
exclude those solutions.

To wrap this in something suitable for demonstration on the web, I
compiled the C++ into wasm using emscripten.  A bare JS program takes
care of allowing user input of the cards and target number, feeding
these to the wasm part, and interpreting the valid solutions in a
human-friendly way.

SOLVER.

TWO EXAMPLES OF THE DIFFICULT ONES FROM YOUTUBE.

Comparison with other solvers: I deliberately didn’t look for details
of how other solvers worked before doing this.  Doing so afterwards
revealed that everyone else had taken the approach of exploring all
valid RPN expressions, with some efforts to remove duplication.  For
example, THIS ONE explains AVOID BY PUSHING BIGGER ONE.  To check my
solver, I wrote a Python program which runs THIS PARTICULAR ONE and
compares the solutions it generates to those my approach generates.
Some care needed to come up with a “canonical” representation of a
given solution, to know when two solutions, different on the surface,
were really “the same”.  EXAMPLE OF TWO EQUIVALENT SOLNS.

Results of this comparison over many random problems was that all
solutions found by my approach were also found by THE OTHER ONE, and
vice versa. Runtime was pretty similar too.  EXACT FIGURES.  Of the
two approaches, the RPN one has the advantage of more obviously being
correct in terms of not missing solutions, so it was reassuring that
the solution sets matched in all the trials I ran.  The RPN solution
did tend to produce duplicate solutions, though.  MEDIAN DUPLICATION
AMOUNT?  For example, the problem

A B C D E F -> G,

when analysed by my solver, has the unique solution BLAH.  The RPN
solver produces the following NNNN variants: BLAH BLAH BLAH.

## Future work

It’s possible to have two cards with the same number.  This solver
will get duplicated solutions if duplicated numbers on cards; fix
this.

Produce something even if we can’t get target exactly.

## Other solvers

* [Rachel Evans](https://github.com/rvedotrc/numbers)

* [Brute Forcing The Countdown Numbers Game —
  Computerphile](https://www.youtube.com/watch?v=cVMhkqPP2YI) also
  uses generation of RPN programs.  A comment says "The first thing
  that sprang to mind was a tree-based approach" but didn't elaborate,
  and replies didn't describe what I've done here.