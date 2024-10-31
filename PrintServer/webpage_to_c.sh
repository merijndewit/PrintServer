#!/bin/bash

DIRECTORY="webpage"

for FILE in "$DIRECTORY"/*.html
do
    BASENAME=$(basename "$FILE" .html)
    
    xxd -i "$FILE" > "$DIRECTORY/$BASENAME.h"
    
    echo "Converted $FILE to $DIRECTORY/$BASENAME.h"
done
