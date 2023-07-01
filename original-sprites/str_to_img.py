from PIL import ImageFont, ImageDraw, Image
import sys

def str_to_img(s: str, fname: str, size=(80,16)) -> Image:
    font = ImageFont.truetype("./Pokemon DPPt.ttf", 16)

    img = Image.new("L", size) ## L=8-bit pixels
    d = ImageDraw.Draw(img)
    d.text((1,1), s, font=font, fill=0x80)
    d.text((0,0), s, font=font, fill=0xff)

    img.save(fname)
           
    return img

           
if __name__ == "__main__":
    if(len(sys.argv) != 3):
        print("Usage: str_to_img.py str out-file")
        sys.exit()
    
    _ = str_to_img(sys.argv[1], sys.argv[2])