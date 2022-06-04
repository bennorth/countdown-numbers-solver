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
};
