import os
import argparse
from struct import *
import re



def convert_image_array_file_to_bin(filename, file_data):
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
        #print("img_data", img_data.group(1))

    c_array = [re.sub("/\*.+\*/", "", m).replace('\n', '').strip() for m in re.findall("#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0(.+?)#endif", file_data, re.S)]

    if c_array:
        c_array = c_array[0]
    else:
        c_array = [re.sub("/\*.+\*/", "", m).replace('\n', '').strip() for m in re.findall("{(.+?)};", file_data, re.S)]
        if c_array:
            c_array = c_array[0]

    if not c_array:
        print("Error: File format not supported")
        return
   
    c_array = c_array.replace('\n', '').replace(' ', '').replace(',', '').replace('0x', '').strip()
    c_array = bytearray.fromhex(c_array)

    if img_header_cf.group(1) == "LV_IMG_CF_TRUE_COLOR_ALPHA":
        img_header_cf = 5
    elif img_header_cf.group(1) == "LV_IMG_CF_TRUE_COLOR":
        img_header_cf = 4
    else:
        print("Error: Color format not supported")
        return
    
    #var $header_32bit = ($lv_cf | (this.w << 10) | (this.h << 21)) >>> 0;
    header_32bit = (img_header_cf| (int(img_header_w.group(1)) << 10) | (int(img_header_h.group(1)) << 21))

    print("Done", header_32bit, len(c_array))
    binary_img = bytearray(len(c_array) + 4)
    binary_img[0] = (header_32bit & 0xFF)
    binary_img[1] = (header_32bit & 0xFF00) >> 8
    binary_img[2] = (header_32bit & 0xFF0000) >> 16
    binary_img[3] = (header_32bit & 0xFF000000) >> 24

    for i in range(len(c_array)):
        binary_img[i+4] = c_array[i]
    
    return binary_img


def convert_from_c_array_img_to_binary(source_dir, target_dir):
    for root, dirs, files in os.walk(source_dir):
        #print(f"root {root} dirs {dirs} files {files}")
        for filename in files:
            if not filename.endswith(".c"):
                continue
            path = os.path.join(root, filename)
            relpath = os.path.relpath(path, start=source_dir)
            print(f"Found {path}")
            with open(path, "r") as infile:
                content = infile.read()
                binary_img = convert_image_array_file_to_bin(filename, content)

                with open(os.path.join(target_dir, filename.split('.')[0] + '.bin'), "wb") as f:
                    f.write(binary_img)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source")
    parser.add_argument("target")
    args = parser.parse_args()

    source_dir = args.source
    target_dir = args.target

    print("Params", source_dir, target_dir)

    convert_from_c_array_img_to_binary(source_dir, target_dir)
