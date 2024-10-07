#!/usr/bin/env bash

# Check if fzf is installed
if ! command -v fzf &> /dev/null; then
  echo "fzf is not installed. Please install it to use this script."
  exit 1
fi

# Check if TRACERGRIND_SEARCH_BASE is set
if [[ -z "$TRACERGRIND_SEARCH_BASE" ]]; then
  TRACERGRIND_SEARCH_BASE="$HOME"
fi

# Prompt the user to select a file using fzf
SOURCE_FILE=$(find "$TRACERGRIND_SEARCH_BASE" -type f | fzf --height 40% --reverse)
DIR=$(dirname "$SOURCE_FILE")
BASENAME=$(basename "$SOURCE_FILE")
RAWNAME=$(basename "$SOURCE_FILE" | cut -d'.' -f1)

TRACE_DIR="$DIR/$BASENAME-traces"
COMPILED_FILENAME="${RAWNAME}.out"
TRACE_FILENAME="${RAWNAME}.trace"
TEXTTRACE_FILENAME="${RAWNAME}.texttrace"
SYMBOLS_FILENAME="${RAWNAME}.elf"

# Check if a file was selected
if [[ -z "$SOURCE_FILE" ]]; then
  echo "No file selected."
  exit 1
fi

mkdir -p "$TRACE_DIR/"
cp "$SOURCE_FILE" "$TRACE_DIR/$BASENAME"
gcc -o "$TRACE_DIR/$COMPILED_FILENAME" "$TRACE_DIR/$BASENAME"
docker run --rm -it -v "$TRACE_DIR":/home tracergrind -d -d --tool=tracergrind --output="/home/$TRACE_FILENAME" "/home/$COMPILED_FILENAME"
docker run --rm -it -v "$TRACE_DIR":/home texttrace "$TRACE_FILENAME" "$TEXTTRACE_FILENAME"
readelf -Wa "$TRACE_DIR/$COMPILED_FILENAME" | grep -e .text -e main > "$TRACE_DIR/$SYMBOLS_FILENAME"
