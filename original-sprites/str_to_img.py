from PIL import ImageFont, ImageDraw, Image
import sys

C_BLACK = 0x00
C_DGREY = 0x40
C_LGREY = 0x80
C_WHITE = 0xff

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
    h = size[1] - 2*yy

    off_x = p+(w-iw)/2 if centred else p
    off_y = p+(h-ih)/2

    if draw_plain_border:
        box = [(p, p), (p+w-bw, p+h-bw)]
        d.rectangle(xy=box, outline=C_BLACK, width=bw)

    if draw_fancy_border:
        box_dark = [(p, p), (p+w-bw, p+h-bw)]
        d.rounded_rectangle(xy=box_dark, radius=2, fill=C_BLACK, outline=C_BLACK, width=bw)

        box_light = [(p+1*bw, p+1*bw), (p+w-2*bw, p+h-2*bw)]
        d.rectangle(xy=box_light, outline=C_DGREY, width=bw)

        box_light = [(p+2*bw, p+2*bw), (p+w-3*bw, p+h-3*bw)]
        d.rectangle(xy=box_light, outline=C_LGREY, width=bw)

        box_white = [(p+2*bw, p+2*bw), (p+w-3*bw, p+h-3*bw)]
        d.rounded_rectangle(xy=box_white, radius=2, fill=C_WHITE, outline=C_WHITE, width=bw)


    d.text((off_x+1,off_y+1), s, font=font, fill=C_LGREY)
    d.text((off_x,off_y+1), s, font=font, fill=C_LGREY)
    d.text((off_x,off_y), s, font=font, fill=C_BLACK)

    img.save(fname)
           
    return img


def generate_picowalker_strings():
    str_to_img("PICOWALKER", "picowalker-plain.bmp", size=(80, 16), padding=0, centred=True,)
    str_to_img("PICOWALKER", "picowalker-border.bmp", size=(80, 16), padding=0, centred=True, draw_plain_border=True)
    str_to_img("PICOWALKER", "picowalker-fancy.bmp", size=(80, 16), padding=1, centred=True, draw_fancy_border=True)

    str_to_img("BAT", "bat-fancy.bmp", size=(32, 16), padding=1, centred=True, draw_fancy_border=True)
           
if __name__ == "__main__":
    #if(len(sys.argv) != 3):
    #    print("Usage: str_to_img.py str out-file")
    #    sys.exit()
    #
    #_ = str_to_img(sys.argv[1], sys.argv[2])
    generate_picowalker_strings()
