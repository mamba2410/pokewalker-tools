#!/bin/sh

# Meant to take in PulseView exported annotations all on one single line.
# Right click on the annotations -> export all annotations

sort -n $1 | grep "X data" | grep -v "bit" | awk '{print $5}' > /tmp/$(basename $1).hex
xxd -r -p -g1 /tmp/$(basename $1).hex > $(basename $1).bin

