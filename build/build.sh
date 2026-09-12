#!/usr/bin/env bash

set -e

mkdir -p bin
g++ -std=c++20 -O3 -Wall -Wextra -pedantic ../src/main.cpp -o bin/kamutoke

echo "Build successful: bin/kamutoke"
