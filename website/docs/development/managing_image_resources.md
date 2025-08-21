---
sidebar_position: 3
---

# Adding new image resources
1. Find .png/.jpg/.svg image.
2. Convert it to a .c file using [LVGL Image Converter](https://lvgl.io/tools/imageconverter) use Color Format `RGB565` or if image have transparent background use `RGB565A8`.
3. Place the `.c` file in an approperiate folder, for example `app/src/images/icons/`
4. Convert the image to binary also running python script `python app/scripts/lvgl_c_array_to_bin_lvgl9.py <path_to_.c_file_in_step_3> app/src/images/binaries/S/`

:::info
    `app/src/images/binaries/S/` is the folder where the binary files are stored to automatically be added when generating the blob of image resources.
:::

To get the images to the watch see [Getting image resources into the watch](image_resources).