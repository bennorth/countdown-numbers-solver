# Development set-up

## Generation of tree programs

``` bash
(
    cd tree-programs
    python3 -m venv venv
    . venv/bin/activate
    pip install --upgrade pip
    pip install pytest click
    deactivate; . venv/bin/activate
    pytest test_compile_trees.py
)
```

(The `deactivate; . venv/bin/activate` ensures the correct `pytest` is
picked up, avoiding problems if a different `pytest` is installed
somewhere else.)

## C++ evaluator (native and wasm)

Having done the above *Generation of tree programs* set-up, make the
tree-based solver and copy it to the comparison tool directory:


``` bash
(
    cd evaluator
    . ../tree-programs/venv/bin/activate
    ./make.sh
    cp evaluator-cli ../compare-solutions/tree-solve
    ./test_evaluator
    ./make-wasm.sh
)
```

(The `make-wasm.sh` step takes some time, because it has to download
the emscripten development kit the first time it's run.)

## RPN-based solver

Make the rpn-based solver and copy it to the comparison tool
directory:

``` bash
(
    cd other-solutions
    ./make.sh
    cp rpn-solve ../compare-solutions
)
```

## Comparison tool

Having done all the above:

``` bash
(
    cd compare-solutions
    python3 -m venv venv
    . venv/bin/activate
    pip install --upgrade pip
    pip install pytest click
    deactivate; . venv/bin/activate
    pytest test_compare_solutions.py
)
```

And then, for example,

``` bash
cd compare-solutions
. venv/bin/activate

python compare_solutions.py --help

python compare_solutions.py show --target 899 --cards 50 6 75 100 10 8
```

## Write-up

Requires [inkscape](https://inkscape.org/).

Having built the wasm version of the tree-based solver, set up the
live-reload editing environment:

``` bash
(
    cd write-up
    ( cd images; ./make.sh )
    npm install livereloadx
)
```

(Warnings about no such file or directory for `package.json` are
expected.)

Then:

* `(cd write-up; ./serve.sh)` to launch the live-reload editing
  system, available at `http://localhost:8211` (which occasionally
  seems flaky), or
* `(cd write-up; ./make.sh)` to do a one-shot build.
