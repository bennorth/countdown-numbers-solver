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
