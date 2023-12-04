import sys
import os
import subprocess
from PIL import Image


def convert_image(path_loc, filename):
    if (
        not filename.endswith(".jpg")
        and not filename.endswith(".JPG")
        and not filename.endswith(".jpeg")
    ):
        return
    image_path = os.path.join(path_loc, os.path.splitext(filename)[0] + ".png")
    if os.path.exists(image_path):
        return

    current_img = Image.open(os.path.join(path_loc, filename))
    print("saving to", image_path)
    current_img.save(
        image_path,
        "PNG",
    )


def resize_image(path_loc, filename):
    if not filename.endswith(".png") and not filename.endswith(".JPG"):
        return
    image_path = os.path.join(path_loc, os.path.splitext(filename)[0] + "_resize.png")
    if os.path.exists(image_path):
        return

    current_img = Image.open(os.path.join(path_loc, filename))
    print("saving to", image_path)
    current_img = current_img.resize((current_img.width // 2, current_img.height // 2))
    current_img.save(
        image_path,
        "PNG",
    )


def cmd_image_r(input_folder, func):
    for filename in os.listdir(input_folder):
        folder = os.path.join(input_folder, filename)
        if os.path.isdir(folder):
            print("is directory", filename)
            cmd_image_r(folder)
        else:
            print(filename)
            func(input_folder, filename)


def create_gif(folder, gif_img):
    img_list = []
    for filename in os.listdir(folder):
        img_list.append(
            (int(filename.removesuffix(".png").removeprefix("rotation_")), filename)
        )

    img_list = sorted(img_list, key=lambda x: x[0])
    images = []
    for filename in img_list:
        print(filename[1])
        images.append(Image.open(os.path.join(folder, filename[1])))
    images[0].save(
        gif_img,
        save_all=True,
        append_images=images[1:],
        duration=100,
        loop=0,
        optimize=True,
    )


cmd = sys.argv[1]
input_path = sys.argv[2]
if cmd == "convert":
    if os.path.isdir(input_path):
        cmd_image_r(input_path, convert_image)
    else:
        convert_image(os.path.dirname(input_path), os.path.basename(input_path))

if cmd == "resize":
    if os.path.isdir(input_path):
        cmd_image_r(input_path, resize_image)
    else:
        resize_image(os.path.dirname(input_path), os.path.basename(input_path))

elif cmd == "gif":
    gif_img = sys.argv[3]
    create_gif(input_path, gif_img)
