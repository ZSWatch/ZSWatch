# External SPI Flash resource storage
Normally try to put all resources in external flash as they quickly fill up the internal flash. This is however
only possible on ZSWatch v4 and later which have external flash on QSPI (fast).
To be compatible with both with and without QSPI flash the code need to use the following macros for declaring images:

Use `ZSW_LV_IMG_DECLARE(file_name)` instead of LVGL's `LV_IMG_DECLARE(file_name)`.<br>
Use `ZSW_LV_IMG_USE(file_name)` instead of `lv_img_set_src(img, &file_name)`.

Those two macros will automatically pick the file from internal flash if no QSPI flash and from QSPI flash if ZSWatch version supports it.
You do however need to convert the image both to `.c` and `.bin` to use this approach right now and place the `.c` in `app/src/images/` folder and the .`bin` in `app/src/images/binaries/lvgl_lfs`

If the resource is big and always shall be in external flash just do `lv_img_set_src(img, "/lvgl_lfs/filename.bin")`
### Folder options

- `filename.bin` put into `lvgl_lfs` goes into littlefs filesystem into one partition of external flash.
    - Usage: `lv_img_set_src(img, "/lvgl_lfs/filename.bin");`
    - Upload: `west upload_fs --type lfs`
- `filename.bin` put into `S` goes into a basic readonly filesystem into one other partition of external flash.
    - Usage: `lv_img_set_src(img, "S:filename.bin");`
    - Upload: `west upload_fs --type raw`

### Which one to use?
Please use littlefs for now those options are mostly for experimentation. Using littlefs may be faster due to littlefs caching. However the other custom filesystem allows us to do more optimization for ZSWatch in the future. 