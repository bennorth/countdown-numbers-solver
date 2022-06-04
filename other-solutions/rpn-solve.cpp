#include <iostream>
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

    void emit_ops()
    {
        bool first_output = true;
        for (auto op : ops)
        {
            if (!first_output)
                std::cout << " ";
            if (const long *val = std::get_if<long>(&op))
                std::cout << "V(" << *val << ")";
            else
                std::cout << op_as_text(std::get<operation>(op));
            first_output = false;
        }
        std::cout << " R\n";
    }

    void search()
    {
        size_t n_eval_stack_elts = eval_stack.size();

        if (n_eval_stack_elts == 1)
        {
            if (eval_stack[0] == target)
                emit_ops();
        }
    }
};
