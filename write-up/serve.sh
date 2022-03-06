#!/bin/bash

cd "$(dirname "${BASH_SOURCE[0]}")" || exit

[ -x ./node_modules/.bin/livereloadx ] || {
    echo Could not find livereloadx
    exit 1
}
#
# The livereloadx tool can be installed with:
#
#     npm install livereloadx

# TERM everything in process group when script exits.
# https://stackoverflow.com/a/2173421
trap 'trap - SIGTERM && kill -- -$$' SIGINT SIGTERM EXIT

mkdir -p dist

(
    cd dist || exit
    exec ../node_modules/.bin/livereloadx --delay 100
) &

(
    cd dist || exit
    exec python3 ../serve.py
) &

cat <<EOF | exec entr ./make.sh dev
index.md
countdown-numbers.css
countdown-numbers.js
EOF
