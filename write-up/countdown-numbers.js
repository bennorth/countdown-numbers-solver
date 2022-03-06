// Take user inputs and feed to solver; display results.

const inputValue = (id) => parseInt(document.getElementById(id).value, 10);
const cardValue = (n) => inputValue(`c${n}`);

const allCards = () => {
    let values = [];
    for (i = 0; i != 6; ++i) {
        values.push(cardValue(i));
    }
    return values;
}

const symbols = {
    "1": { nonInv: "×", inv: "÷" },
    "2": { nonInv: "+", inv: "−" },
};

const pprintPrograms = (opcode_u8s) => {
    const cards = allCards();
    let exprns = [];
    let ops = [];
    for (let u8Idx = 0; u8Idx !== opcode_u8s.length; u8Idx += 3) {
        const opcode = opcode_u8s[u8Idx];
        const arg0 = opcode_u8s[u8Idx + 1];
        let arg1 = opcode_u8s[u8Idx + 2];

        switch (opcode) {
        case 0: // Value
            ops.push(`${cards[arg0]}`);
            break;
        case 1: // Multiply } nArgs = arg0
        case 2: // Add      } nonInvMask = arg1
            {
                const symInv = symbols[opcode].inv;
                const symNonInv = symbols[opcode].nonInv;

                let invInputs = [];
                let nonInvInputs = [];
                for (let operandIdx = 0; operandIdx != arg0; ++operandIdx, arg1 >>= 1) {
                    const isNonInv = arg1 & 1;
                    (isNonInv ? nonInvInputs : invInputs).push(ops.pop());
                }
                invInputs.sort((a, b) => (b - a));
                nonInvInputs.sort((a, b) => (b - a));
                let invSuffix;
                switch (invInputs.length) {
                case 0:
                    invSuffix = "";
                    break;
                case 1:
                    invSuffix = ` ${symInv} ${invInputs[0]}`;
                    break;
                default:
                    invSuffix = ` ${symInv} (${invInputs.join(` ${symNonInv} `)})`;
                    break;
                }
                ops.push(`(${nonInvInputs.join(` ${symNonInv} `)}${invSuffix})`);
            }
            break;
        case 3: // Return
            const fullExprn = ops.pop();
            // Strip outermost ()s:
            exprns.push(fullExprn.substr(1, fullExprn.length - 2));
            break;
        }
    }
    return exprns;
};
