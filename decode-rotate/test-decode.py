#!/usr/bin/env python

import sys
from os import popen
from PIL import Image, ImageDraw

def main(width, height, infile, outfile):

    ## yeah yeah, security fans chill out
    print(popen(f"./decode-rotate {width} {height} {infile} {outfile}").read())

    ## if it decoded ok, get output contents
    ## TODO: check if it decoded ok
    with open(outfile, "rb") as f:
        contents = f.read()
        print(len(contents))

    ## Convert contents to image

    ## display image

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Error: Need 5 arguments")
        exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])

