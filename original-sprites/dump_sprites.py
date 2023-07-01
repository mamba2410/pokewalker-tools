import ndspy.narc
import ndspy.lz10
from bin_to_img import *
import os.path

def dump(path):
    narc = ndspy.narc.NARC.fromFile(path)
    bname = os.path.basename(path)
    print("Found {} files".format( len(narc.files)))

    for id in range(0, len(narc.files)):
        try:
            raw = ndspy.lz10.decompress(narc.files[id])
        except:
            raw = narc.files[id]

        print("file {}: {} bytes".format(id, len(bytearray(raw))))
        with open(f"./out/{bname}_{id:03d}.bin", "wb") as f:
            f.write(bytearray(raw))
        try:
            img = unslice_image(bin_to_img(raw))
            img.save(f"./out/{bname}_{id:03d}.png", "PNG")
        except Exception as e:
            print("Error for file {}".format(id))
            print(e)
    print("Done")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Correct syntax: sprite_dump.py in-file")
        sys.exit()
    dump(sys.argv[1])
