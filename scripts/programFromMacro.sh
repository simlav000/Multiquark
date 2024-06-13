#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <ExampleMacro>"
    exit 1
fi

ExampleMacro="$1"
OutputFile="${ExampleMacro%.*}"

g++ -o "$OutputFile" "$ExampleMacro" `root-config --cflags --libs`

if [ $? -eq 0 ]; then
    echo "Compilation successful. Output file: $OutputFile"
else 
    echo "Compilation failed."
fi
