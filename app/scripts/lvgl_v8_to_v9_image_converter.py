import os
import argparse
from struct import *
import re


def convert_image_array(filename, file_data):
    print("--------------------")
    print(filename)
    print("--------------------")

    img_name_r = re.compile(r"const lv_img_dsc_t (.*?) = {")
    img_header_cf_r = re.compile(r".header.cf = (.*?),")
    img_header_always_zero_r = re.compile(r".header.always_zero = (.*?),")
    img_header_reserved_r = re.compile(r".header.reserved = (.*?),")
    img_header_w_r = re.compile(r".header.w = (.*?),")
    img_header_h_r = re.compile(r".header.h = (.*?),")
    img_data_size_r = re.compile(r".data_size = (.*?),")
    img_data_r = re.compile(r".data = (.*?),")

    img_name = img_name_r.search(file_data)
    if img_name:
        print("img_name", img_name.group(1))
    else:
        print("Error: Image name not found")
        return

    img_header_cf = img_header_cf_r.search(file_data)
    if img_header_cf:
        print("img_header_cf", img_header_cf.group(1))

    img_header_always_zero = img_header_always_zero_r.search(file_data)
    if img_header_always_zero:
        print("img_header_always_zero", img_header_always_zero.group(1))

    img_header_reserved = img_header_reserved_r.search(file_data)
    if img_header_reserved:
        print("img_header_reserved", img_header_reserved.group(1))

    img_header_w = img_header_w_r.search(file_data)
    if img_header_w:
        print("img_header_w", img_header_w.group(1))

    img_header_h = img_header_h_r.search(file_data)
    if img_header_h:
        print("img_header_h", img_header_h.group(1))

    img_data_size = img_data_size_r.search(file_data)
    if img_data_size:
        print("img_data_size", img_data_size.group(1))

    img_data = img_data_r.search(file_data)
    
    if img_data:
        pass
        # print("img_data", img_data.group(1))

    c_array = [
        re.sub(r"/\*.+\*/", "", m).replace("\n", "").strip()
        for m in re.findall(
            "#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0(.+?)#endif",
            file_data,
            re.S,
        )
    ]

    if c_array:
        c_array = c_array[0]
    else:
        c_array = [
            re.sub(r"/\*.+\*/", "", m).replace("\n", "").strip()
            for m in re.findall("{(.+?)};", file_data, re.S)
        ]
        if c_array:
            c_array = c_array[0]

    if not c_array:
        print("Error: File format not supported")
        return

    c_array = (
        c_array.replace("\n", "")
        .replace(" ", "")
        .replace(",", "")
        .replace("0x", "")
        .strip()
    )
    image_data = bytearray.fromhex(c_array)
    has_alpha = False

    if img_header_cf.group(1) == "LV_IMG_CF_TRUE_COLOR_ALPHA":
        has_alpha = True
    elif img_header_cf.group(1) == "LV_IMG_CF_TRUE_COLOR":
        has_alpha = False
    else:
        print("Error: Color format not supported")
        return

    # Step 2: Separate the RGB565 and alpha channels
    #
    if has_alpha:
        print("has alpha channel")
        rgb565_data, alpha_data = separate_rgb565_and_alpha(image_data)
    else:
        print("no alpha channel")
        rgb565_data = exract_rgb565(image_data)
        alpha_data = []

    # Step 3: Write the data to a new C file
    return replace_data_in_c_code(file_data, rgb565_data, alpha_data)


def replace_data_in_c_code(infile_content, rgb565_data, alpha_data):

    pattern = re.compile(r"\[\]\s*=\s*\{.*?\};", re.DOTALL)

    hex_str = ""
    x = 0
    for rgb in rgb565_data:
        # Combine the two bytes into a single uint16_t value
        value = (rgb[0] << 8) | rgb[1]

        hex_str = hex_str + (f"0x{rgb[1]:02X},")
        hex_str = hex_str + (f"0x{rgb[0]:02X},")
        x = x + 1
        if x % 48 == 0:
            hex_str = hex_str + ("\n")

    hex_str = hex_str + ("\n")
    x = 0
    # Write the Alpha data as a C array
    for alpha in alpha_data:
        hex_str = hex_str + (f"0x{alpha:02X},")
        x = x + 1
        if x % 96 == 0:
            hex_str = hex_str + ("\n")
    hex_str = "    " + hex_str.replace("\n", "\n    ")

    new_content = pattern.sub(
        f"[] = {{\n{hex_str}\n}}; // LVGL_9 compatible", infile_content
    )
    #print(new_content)
    new_content = (
        new_content.replace(
            "LV_IMG_CF_TRUE_COLOR_ALPHA", "LV_COLOR_FORMAT_NATIVE_WITH_ALPHA"
        )
        .replace("LV_IMG_CF_TRUE_COLOR", "LV_COLOR_FORMAT_NATIVE")
        .replace(".header.always_zero = 0,", ".header.magic = LV_IMAGE_HEADER_MAGIC,")
        .replace(".header.reserved = 0,", "")
        .replace("LV_IMG_PX_SIZE_ALPHA_BYTE", "LV_COLOR_NATIVE_WITH_ALPHA_SIZE")
        .replace("LV_COLOR_SIZE", "sizeof(lv_color_t)")
    )

    return new_content.encode("utf-8")


def separate_rgb565_and_alpha(image_data):
    rgb565_array = []
    alpha_array = []

    # Loop through the image data, processing 3 bytes (RGB565 + alpha) per pixel
    for i in range(0, len(image_data), 3):
        rgb565 = image_data[i : i + 2]  # 2 bytes for RGB565
        alpha = image_data[i + 2]  # 1 byte for alpha

        # Append to corresponding arrays
        rgb565_array.append(rgb565)
        alpha_array.append(alpha)

    return rgb565_array, alpha_array


def exract_rgb565(image_data):
    rgb565_array = []
    print(len(image_data))

    # Loop through the image data, processing 2 bytes (RGB565) per pixel
    for i in range(0, len(image_data), 2):
        rgb565 = image_data[i : i + 2]  # 2 bytes for RGB565
        # Append to corresponding arrays
        rgb565_array.append(rgb565)

    return rgb565_array


def convert_from_v8_c_array_to_v9(source_path, target_path):
    if os.path.isfile(source_path):
        # Single file mode
        filename = os.path.basename(source_path)
        if not filename.endswith(".c") or "font" in filename:
            print(f"Skipping {filename}")
            return
        with open(source_path, "r") as infile:
            content = infile.read()
            binary_img = convert_image_array(filename, content)
            with open(target_path, "wb") as f:
                f.write(binary_img)
    else:
        # Directory mode
        for root, dirs, files in os.walk(source_path):
            print(f"root {root} dirs {dirs} files {files}")
            for filename in files:
                print(f"filename {filename}")
                if not filename.endswith(".c") or "font" in filename:
                    continue
                path = os.path.join(root, filename)
                relpath = os.path.relpath(path, start=source_path)
                print(f"Found {path}")
                with open(path, "r") as infile:
                    content = infile.read()
                    binary_img = convert_image_array(filename, content)
                    if binary_img:
                        out_dir = os.path.join(target_path, os.path.dirname(relpath))
                        os.makedirs(out_dir, exist_ok=True)
                        with open(os.path.join(out_dir, filename), "wb") as f:
                            f.write(binary_img)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source")
    parser.add_argument("target")
    args = parser.parse_args()

    source_dir = args.source
    target_dir = args.target

    convert_from_v8_c_array_to_v9(source_dir, target_dir)
