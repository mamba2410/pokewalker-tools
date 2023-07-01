import PIL
from PIL import Image
import sys

# Makes a lambda that only accepts given colors (ie, returns black for them, white otherwise)
def make_transform(accepted_colors):
    return (lambda color: 0 if color in accepted_colors else 255)

def slice_image(in_img, width=64, height=48, frames=1):
    in_img = in_img.convert('L').transpose(Image.ROTATE_270)
    print(width, height, frames)
    print(in_img.width, in_img.height)

    out_img = Image.new('1', (16,width*height*frames//8))

    # A bit fugly, but it just separates the two transforms and makes a chunk list out of them.
    left_in_img = in_img.point(make_transform([0,128]))
    right_in_img = in_img.point(make_transform([0,64]))
    lchunks = [right_in_img.crop((in_img.width-((i+1)*8),0,in_img.width-(i*8),width)) for i in range(0, frames*height//8)]
    rchunks = [left_in_img.crop((in_img.width-((i+1)*8),0,in_img.width-(i*8),width)) for i in range(0, frames*height//8)]

    for i in range(0,frames*height//8):
        out_img.paste(lchunks[i], (0, i*width))
        out_img.paste(rchunks[i], (8, i*width))
        
        #if not animated:
        #    out_img.paste(lchunks[i], (0, (i+6)*64))
        #    out_img.paste(rchunks[i], (8, (i+6)*64))
    
    #out_img = Image.frombytes('L', (height*frames, width), out_img.tobytes())
    return out_img
        
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Correct syntax: slice_image.py in-file [out-file]")
        sys.exit()
        
    with Image.open(sys.argv[1]) as in_img:
        out_img = slice_image(in_img)
        
    if len(sys.argv) > 2:
        out_img.save(sys.argv[2], "PNG")
    else:
        out_img.show()
    