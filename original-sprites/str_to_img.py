from PIL import ImageFont, ImageDraw, Image
import numpy as np
import sys

C_BLACK = 0x00
C_DGREY = 0x40
C_LGREY = 0x80
C_WHITE = 0xff


# Yoinked from https://stackoverflow.com/questions/46385999/transform-an-image-to-a-bitmap
def img_to_bmp(ifname: str, ofname: str):
    img = Image.open(ifname)
    a = np.array(img)
    r, g, b = np.split(a, 3, axis=2)
    r = r.reshape(-1)
    g = g.reshape(-1)
    b = b.reshape(-1)

    bitmap = list(map(lambda x: 0.299*x[0] + 0.587*x[1] + 0.114*x[2], zip(r, g, b)))
    bitmap = np.array(bitmap).reshape([a.shape[0], a.shape[1]])
    bmp = Image.fromarray(bitmap.astype(np.uint8))
    if ofname is not None:
        bmp.save(ofname)
    return bmp

def str_to_img(s: str, fname: str, size=(80,16), fontsize=10, padding=1, centred=False, draw_plain_border=False, draw_fancy_border=False) -> Image:
    #font = ImageFont.truetype("./Pokemon DPPt.ttf", 16)
    font = ImageFont.truetype("./pkmndp.ttf", fontsize)

    img = Image.new("L", size) ## L=8-bit pixels
    d = ImageDraw.Draw(img)
    _, _, iw, ih = d.textbbox((0, 0), s, font=font)

    p = padding
    bw = 1

    d.rectangle([(0,0), (size[0]-1, size[1]-1)], fill=C_WHITE, outline=C_WHITE)

    px, py = 2*p, p
    w = size[0] - 2*px
    h = size[1] - 2*py

    off_x = px+(w-iw)/2 if centred else px
    off_y = py+(h-ih)/2

    if draw_plain_border:
        box = [(px, py), (px+w-bw, py+h-bw)]
        d.rectangle(xy=box, outline=C_BLACK, width=bw)

    if draw_fancy_border:
        box_dark = [(px, py), (px+w-bw, py+h-bw)]
        d.rounded_rectangle(xy=box_dark, radius=2, fill=C_BLACK, outline=C_BLACK, width=bw)

        box_light = [(px+1*bw, py+1*bw), (px+w-2*bw, py+h-2*bw)]
        d.rectangle(xy=box_light, outline=C_DGREY, width=bw)

        box_light = [(px+2*bw, py+2*bw), (px+w-3*bw, py+h-3*bw)]
        d.rectangle(xy=box_light, outline=C_LGREY, width=bw)

        box_white = [(px+2*bw, py+2*bw), (px+w-3*bw, py+h-3*bw)]
        d.rounded_rectangle(xy=box_white, radius=2, fill=C_WHITE, outline=C_WHITE, width=bw)


    d.text((off_x+1,off_y+1), s, font=font, fill=C_LGREY)
    d.text((off_x,off_y+1), s, font=font, fill=C_LGREY)
    d.text((off_x,off_y), s, font=font, fill=C_BLACK)

    img.save(fname)
           
    return img


def str_to_img_chars(s, fname, size=(80, 16), centred=True, draw_plain_border=False, draw_fancy_border=False):
    img = Image.new("L", size) ## L=8-bit pixels
    simg = Image.new("L", size)
    d = ImageDraw.Draw(img)

    p = 1 if draw_fancy_border else 0
    bw = 1

    d.rectangle([(0,0), (size[0]-1, size[1]-1)], fill=C_WHITE, outline=C_WHITE)

    px, py = 2*p, p
    w = size[0] - 2*px
    h = size[1] - 2*py


    if draw_plain_border:
        box = [(px, py), (px+w-bw, py+h-bw)]
        d.rectangle(xy=box, outline=C_BLACK, width=bw)

    if draw_fancy_border:
        box_dark = [(px, py), (px+w-bw, py+h-bw)]
        d.rounded_rectangle(xy=box_dark, radius=2, fill=C_BLACK, outline=C_BLACK, width=bw)

        box_light = [(px+1*bw, py+1*bw), (px+w-2*bw, py+h-2*bw)]
        d.rectangle(xy=box_light, outline=C_DGREY, width=bw)

        box_light = [(px+2*bw, py+2*bw), (px+w-3*bw, py+h-3*bw)]
        d.rectangle(xy=box_light, outline=C_LGREY, width=bw)

        box_white = [(px+2*bw, py+2*bw), (px+w-3*bw, py+h-3*bw)]
        d.rounded_rectangle(xy=box_white, radius=2, fill=C_WHITE, outline=C_WHITE, width=bw)

    cx = 0
    for c in s:
        if (c >= 'A') & (c <= 'Z'):
            cname = f"./walker_font/upper{c}.png"
        elif (c >= 'a') & (c <= 'z'):
            cname = f"./walker_font/lower{c+32}.png"
        elif (c >= '0') & (c <= '9'):
            cname = f"./walker_font/{c}.png"
        else:
            raise ValueError(f"Don't have image for character '{c}'")


        #cimg = img_to_bmp(cname, None)
        cimg = Image.open(cname)
        cimg = cimg.crop((0, 0, cimg.size[0], 9))
        print(f"Using {cname} {cimg.size}")
        #d.bitmap((cx, 0), cimg)
        simg.paste(cimg, box=(cx, 0))
        cx += cimg.size[0]

    iw = cx
    ih = 9
    off_x = int(np.round(px+(w-iw)/2)) if centred else px
    off_y = int(np.round(py+(h-ih)/2))

    simg = simg.crop((0, 0, iw, ih))
    img.paste(simg, box=(off_x, off_y))

    img.save(fname)


def generate_picowalker_strings():
    str_to_img_chars("PICOWALKER", "picowalker-plain.bmp", size=(80, 16), centred=True,)
    str_to_img_chars("PICOWALKER", "picowalker-border.bmp", size=(80, 16), centred=True, draw_plain_border=True)
    str_to_img_chars("PICOWALKER", "picowalker-fancy.bmp", size=(88, 16), centred=True, draw_fancy_border=True)

    str_to_img_chars("BATTERY", "bat-fancy.bmp", size=(48, 16), centred=True, draw_fancy_border=True)
    str_to_img_chars("%", "percent.bmp", size=(8, 16), centred=True)
           
if __name__ == "__main__":
    #if(len(sys.argv) != 3):
    #    print("Usage: str_to_img.py str out-file")
    #    sys.exit()
    #
    #_ = str_to_img(sys.argv[1], sys.argv[2])
    generate_picowalker_strings()
    #img_to_bmp("picowalker-fancy-zenith.png", "picowalker-fancy-zenith.bmp")

