#include <algorithm>
#include <variant>
#include <vector>

enum class operation
{
    op_add,
    op_subtract,
    op_multiply,
    op_divide
};

const char *op_as_text(operation op)
{
    switch (op)
    {
    case operation::op_add:
        return "A(++)";
    case operation::op_subtract:
        return "A(-+)";
    case operation::op_multiply:
        return "M(++)";
    case operation::op_divide:
        return "M(-+)";
    default:
        return "?????";
    }
}

using longs = std::vector<long>;

using instruction = std::variant<long, operation>;
using instructions = std::vector<instruction>;

struct search_state
{
    long target;
    instructions ops;
    longs eval_stack;
    longs unused_cards;

    search_state(size_t n_int_args, char **int_args)
    {
        target = atol(int_args[0]);
        std::transform(
            int_args + 1, int_args + n_int_args,
            std::back_inserter(unused_cards),
            [](const char *x)
            { return atol(x); });

        size_t n_cards = unused_cards.size();
        ops.reserve(2 * n_cards);
        eval_stack.reserve(n_cards);
    }
};
