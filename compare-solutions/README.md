# Comparing solutions

## Development set-up

``` bash
python3 -m venv venv
. venv/bin/activate
pip install --upgrade pip
pip install pytest
```

Compile the two solvers to be compared and copy them here under the
expected names:

``` bash
(
    cd ../evaluator
    ./make.sh
    cp evaluator-cli ../compare-solutions/tree-solve
)

(
    cd ../other-solutions
    ./make.sh
    cp rpn-solve ../compare-solutions
)
```
