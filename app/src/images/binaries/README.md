# External flash resource storage
Images and other resources are stored in external flash, as they would quickly fill up the internal flash.

- `filename.bin` put into `S` goes into a basic read-only filesystem in one partition of external flash.
    - Usage: `lv_img_set_src(img, ZSW_LV_IMG_USE(filename));` or `lv_img_set_src(img, "S:filename.bin");`
    - Upload: `west upload_fs --type raw`
    - native_sim links the same filesystem image into the executable at build time, nothing to upload.
- `filename.bin` put into `lvgl_lfs` goes into a littlefs filesystem in another partition of external flash.
    - Usage: `lv_img_set_src(img, "/lvgl_lfs/filename.bin");`
    - Upload: `west upload_fs --type lfs`

### Which one to use?
Use `S`. Files are found through a hash index in flash, and while the flash is memory mapped (XIP on, which it is
when the display is on) LVGL draws the images straight from flash without copying them to RAM.

See https://zswatch.dev/docs/development/image_resources for how to add an image.
