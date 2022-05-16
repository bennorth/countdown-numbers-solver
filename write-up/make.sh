#!/bin/bash

cd "$(dirname "${BASH_SOURCE[0]}")" || exit

mkdir -p dist

GITHUB_MARKDOWN_CSS=dist/github-markdown.css
GITHUB_MARKDOWN_CSS_URL=https://raw.githubusercontent.com/sindresorhus/github-markdown-css/gh-pages/github-markdown.css

if [ ! -f "${GITHUB_MARKDOWN_CSS}" ]; then
    curl -o "${GITHUB_MARKDOWN_CSS}" "${GITHUB_MARKDOWN_CSS_URL}"
fi

EXTRA_SCRIPT_ARGS=()
if [ "$1" = "dev" ]; then
    EXTRA_SCRIPT_ARGS=(-A ./include-livereload.html)
fi

pandoc -s \
       -f gfm \
       --template template.html \
       -o dist/index.html \
       -c ./github-markdown.css \
       -c ./countdown-numbers.css \
       "${EXTRA_SCRIPT_ARGS[@]}" \
       --metadata title="Finding solutions to the Countdown numbers round" \
       index.md

cp ../evaluator/CountdownSolver.js ../evaluator/CountdownSolver.wasm dist
cp countdown-numbers.css countdown-numbers.js dist

mkdir -p dist/images

for img in \
    sample-add-mult-tree.png \
    sample-add-mult-tree-with-inverses.png \
    sample-add-mult-tree-with-inverses-2.png \
    sample-indexed-tree-2.png \
    Countdown-numbers-952.jpg \
    Countdown-numbers-813.jpg \
    ; do
    cp images/"${img}" dist/images
done
