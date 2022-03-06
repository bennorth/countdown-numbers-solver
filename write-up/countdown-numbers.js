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
