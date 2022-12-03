import sys
import os
import subprocess
from PIL import Image


# grab first and second argument

input_path = sys.argv[1]

# if the output folder does not exist - it creates it
# try:
#    os.mkdir(os.getcwd() + '\\' + output_path)
#    print(f'The output folder - {output_path}, has been created! \n')
# except FileExistsError:
#    print(
#        f'The folder {output_path} already exists! All the PNG files will be saved in it! \n')
#
# loops through the InputJPG
def convert_image(path_loc, filename):
    if not filename.endswith(".jpg") and not filename.endswith(".JPG"):
        return
    image_path = os.path.join(
        "./images/pngs", os.path.splitext(filename)[0] + "_high.png"
    )
    if os.path.exists(image_path):
        return

    current_img = Image.open(os.path.join(path_loc, filename))
    # print('Working on image: ' + os.path.splitext(filename)[0])
    # print(
    #    f'Format: {current_img.format}, Size: {current_img.size}, Mode: {current_img.mode}')

    # imgtool = ".\\build\\Visual Studio Build Tools 2022 Release - amd64_x86\\\Release\\\external\\pbrt-v3\\imgtool.exe"
    # args = [imgtool, "convert", os.path.join (path_loc, filename), image_path]
    # subprocess.run(args)
    # coverts the images to PNG + saves to the output folder
    width, height = current_img.size
    print("saving to", image_path)
    # current_img.thumbnail((width // 2, height // 2), Image.Resampling.LANCZOS)
    current_img.save(
        image_path,
        "PNG",
    )


def convert_image_r(input_folder):
    for filename in os.listdir(input_folder):
        folder = os.path.join(input_folder, filename)
        if os.path.isdir(folder):
            print("is directory", filename)
            convert_image_r(folder)
        else:
            print(filename)
            convert_image(input_folder, filename)


convert_image_r(input_path)


def create_gif(folder):
    img_list = []
    for filename in os.listdir(folder):
        img_list.append(Image.open(os.path.join(folder, filename)))
    img_list[0].save(
        "test.gif",
        save_all=True,
        append_images=img_list[1:],
        duration=100,
        loop=0,
        optimize=True,
    )


# create_gif(input_path)
