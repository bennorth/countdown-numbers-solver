#!/bin/bash

command -v inkscape > /dev/null || {
    echo Could not find inkscape
    exit 1
}

for stem in \
    sample-add-mult-tree \
    sample-add-mult-tree-with-inverses-2 \
    sample-add-mult-tree-with-inverses \
    ; do
    inkscape \
        --export-type=png \
        --export-area-drawing \
        -o "${stem}".png \
        "${stem}.svg"
done

