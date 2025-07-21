#!/bin/bash

# === Config: path to g++ ===
GPP="/usr/bin/g++"  # Adjust if needed

# === Check if user provided a .cpp file ===
if [ -z "$1" ]; then
  echo "Usage: $0 [source_file.cpp] [args...]"
  exit 1
fi

FILE="$1"
NAME="${FILE%.*}"
shift

# Create build directory if it doesn't exist
mkdir -p build

echo "Compiling $FILE..."
"$GPP" -std=c++17 -fdiagnostics-color=always -g "$FILE" \
  -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror \
  -o "build/$NAME"

if [ $? -ne 0 ]; then
  echo "Build failed."
  exit $?
fi

echo "Build succeeded. Running with arguments: $*"
"./build/$NAME" "$@"
