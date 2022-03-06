#!/bin/bash

cd "$(dirname "${BASH_SOURCE[0]}")" || exit

if [ ! -r ./emsdk/emsdk_env.sh ]; then
    # Instructions from
    #
    #   https://emscripten.org/docs/getting_started/downloads.html
    #
    git clone https://github.com/emscripten-core/emsdk.git
    (
        cd emsdk || exit
        ./emsdk install latest
        ./emsdk activate latest
    )
fi

if [ ! -r ./emsdk/emsdk_env.sh ]; then
    echo Emscripten SDK not found
    exit
fi

# shellcheck disable=SC1091
source ./emsdk/emsdk_env.sh
