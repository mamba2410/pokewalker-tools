import sys
from PIL import Image

WIDTH = 64
HEIGHT = 48
FRAMES = 2
    
def bin_to_img(in_bin, width=WIDTH, height=HEIGHT, frames=FRAMES):
    inverted = bytes([b ^ 0b11111111 for b in in_bin])
    return Image.frombytes('1', (16, frames*width*height//8), inverted)
    
def unslice_image(in_img, width=WIDTH, height=HEIGHT, frames=FRAMES):
    if in_img == None:
        return in_img
    print(width, height, frames)
    print(in_img.width, in_img.height)

    in_img = in_img.convert('L')

    lchunks = [in_img.crop((0,in_img.height-((i+1)*width),8,in_img.height-(i*width))) for i in range(0, frames*height//8) ]
    rchunks = [in_img.crop((8,in_img.height-((i+1)*width),16,in_img.height-(i*width))) for i in range(0, frames*height//8)]
    
    out_img = Image.new('L', (height*frames,width))

    for i in range(0, frames*height//8):
        out_img.paste(Image.blend(lchunks[i],rchunks[i],1/3), (i*8, 0))
    
    return out_img.transpose(Image.ROTATE_90)
    
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Correct syntax: bin_to_img.py in-file")
        sys.exit()
    with open(sys.argv[1], "rb") as in_bin:
        unslice_image(bin_to_img(in_bin.read())).show()
