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

        if (n_eval_stack_elts >= 2)
        {
            long v1 = eval_stack.back();
            eval_stack.pop_back();
            long v2 = eval_stack.back();
            auto &eval_stack_top = eval_stack.back();

            ops.push_back(operation::op_add); // placeholder
            auto &op_stack_top = std::get<operation>(ops.back());

            if (v1 >= v2)
            {
                op_stack_top = operation::op_add;
                eval_stack_top = v1 + v2;
                search();
            }

            if (v1 > v2)
            {
                op_stack_top = operation::op_subtract;
                eval_stack_top = v1 - v2;
                search();
            }

            if (v1 >= v2 && v2 > 1)
            {
                op_stack_top = operation::op_multiply;
                eval_stack_top = v1 * v2;
                search();
            }

            if (v2 > 1 && (v1 % v2) == 0)
            {
                op_stack_top = operation::op_divide;
                eval_stack_top = v1 / v2;
                search();
            }

            ops.pop_back();

            eval_stack_top = v2;
            eval_stack.push_back(v1);
        }

        size_t n_unused_cards = unused_cards.size();
        if (n_unused_cards > 0)
        {
            long last_card = unused_cards.back();
            for (size_t i = 0; i < n_unused_cards; ++i)
            {
                long try_card = unused_cards[i];

                bool already_tried = false;
                for (size_t j = 0; j < i; ++j)
                    if (unused_cards[j] == try_card)
                    {
                        already_tried = true;
                        break;
                    }

                if (already_tried)
                    continue;

                ops.push_back(try_card);
                eval_stack.push_back(try_card);

                unused_cards[i] = last_card;
                unused_cards.pop_back();

                search();

                unused_cards.push_back(last_card);
                unused_cards[i] = try_card;

                eval_stack.pop_back();
                ops.pop_back();
            }
        }
    }
};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "usage: rpn-solve TARGET CARD_1 ... CARD_N\n";
        return EXIT_FAILURE;
    }

    search_state(argc - 1, argv + 1).search();

    return EXIT_SUCCESS;
}
