import os
import argparse
import re
import struct

class ColorFormat:
    # Minimal enum-like class for color formats
    ARGB8888 = 0x10
    XRGB8888 = 0x11
    RGB565 = 0x12
    NATIVE = 0x12
    ARGB8565 = 0x13
    RGB565A8 = 0x14
    NATIVE_WITH_ALPHA = 0x14
    RGB888 = 0x0F
    L8 = 0x06
    I1 = 0x07
    I2 = 0x08
    I4 = 0x09
    I8 = 0x0A
    A1 = 0x0B
    A2 = 0x0C
    A4 = 0x0D
    A8 = 0x0E
    RAW = 0x01
    RAW_ALPHA = 0x02

    @classmethod
    def from_string(cls, s):
        return getattr(cls, s, None)

class LVGLImageHeader:
    def __init__(self, cf, w, h, stride, flags=0, reserved_2=0):
        self.magic = 0x19
        self.cf = cf
        self.flags = flags
        self.w = w
        self.h = h
        self.stride = stride
        self.reserved_2 = reserved_2

    @property
    def binary(self):
        # <BBHHHH for little endian: magic, cf, flags, w, h, stride, reserved_2
        return struct.pack('<BBHHHHH', self.magic, self.cf, self.flags, self.w, self.h, self.stride, self.reserved_2)

class LVGLImage:
    def __init__(self):
        self.cf = None
        self.w = 0
        self.h = 0
        self.stride = 0
        self.flags = 0
        self.reserved_2 = 0
        self.data = b''

    def set_data(self, cf, w, h, data, stride=0, flags=0, reserved_2=0):
        self.cf = cf
        self.w = w
        self.h = h
        self.stride = stride
        self.flags = flags
        self.reserved_2 = reserved_2
        self.data = data

    @property
    def header(self):
        return LVGLImageHeader(self.cf, self.w, self.h, self.stride, self.flags, self.reserved_2).binary

    def to_bin(self, filename):
        with open(filename, "wb") as f:
            f.write(self.header)
            f.write(self.data)

def get_stride(w, cf):
    bpp_map = {
        ColorFormat.L8: 8,
        ColorFormat.I8: 8,
        ColorFormat.A8: 8,
        ColorFormat.ARGB8888: 32,
        ColorFormat.XRGB8888: 32,
        ColorFormat.RGB565: 16,
        ColorFormat.RGB565A8: 16,  # 16bpp + a8 map
        ColorFormat.ARGB8565: 24,
        ColorFormat.RGB888: 24,
        # Add more formats as needed
    }

    bpp = bpp_map.get(cf, 0)

    return int (w * (bpp / 8))

def parse_lvgl_c_array(file_data):
    img_header_cf_r = re.compile(r"\.header\.cf\s*=\s*(LV_COLOR_FORMAT_\w+)", re.S)
    img_header_w_r = re.compile(r"\.header\.w\s*=\s*(\d+)", re.S)
    img_header_h_r = re.compile(r"\.header\.h\s*=\s*(\d+)", re.S)
    img_data_r = re.compile(r"uint8_t\s+\w+\[\]\s*=\s*\{(.+?)\};", re.S)

    cf_match = img_header_cf_r.search(file_data)
    w_match = img_header_w_r.search(file_data)
    h_match = img_header_h_r.search(file_data)
    data_match = img_data_r.search(file_data)

    if not (cf_match and w_match and h_match and data_match):
        print("Error: Could not parse LVGL C array file")
        return None

    cf_str = cf_match.group(1).replace("LV_COLOR_FORMAT_", "")
    cf = ColorFormat.from_string(cf_str)
    if cf is None:
        print(f"Error: Unsupported color format {cf_str}")
        return None

    w = int(w_match.group(1))
    h = int(h_match.group(1))
    stride = get_stride(w, cf)
    data_str = data_match.group(1)
    data_str = re.sub(r"/\*.*?\*/", "", data_str)
    data_str = data_str.replace("\n", "").replace(" ", "")
    data_bytes = bytearray(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", data_str))

    return cf, w, h, stride, data_bytes

def convert_c_array_file_to_bin(filepath, target_dir):
    filename = os.path.basename(filepath)
    with open(filepath, "r") as infile:
        content = infile.read()
        print(f"Processing {filename}...")
        result = parse_lvgl_c_array(content)
        if not result:
            return
        cf, w, h, stride, data_bytes = result
        img = LVGLImage()
        img.set_data(cf, w, h, data_bytes, stride)
        bin_path = os.path.join(target_dir, filename.split(".")[0] + ".bin")
        img.to_bin(bin_path)
        print(f"Converted {filename} -> {bin_path}")

def convert_c_array_to_bin(source, target_dir):
    if os.path.isfile(source):
        if source.endswith(".c") and "font" not in source:
            convert_c_array_file_to_bin(source, target_dir)
    elif os.path.isdir(source):
        for root, dirs, files in os.walk(source):
            for filename in files:
                if not filename.endswith(".c") or "font" in filename:
                    continue
                path = os.path.join(root, filename)
                convert_c_array_file_to_bin(path, target_dir)
    else:
        print(f"Error: {source} is not a valid file or directory")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", help="Source .c file or folder")
    parser.add_argument("target", help="Target folder for .bin files")
    args = parser.parse_args()

    source = args.source
    target_dir = args.target

    convert_c_array_to_bin(source, target_dir)