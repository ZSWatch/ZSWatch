# External SPI Flash resource storage

## Folder options

- `filename.bin` put into `lvgl_lfs` goes into littlefs filesystem into one partition of external flash.
    - Usage: `lv_img_set_src(img, "/lvgl_lfs/filename.bin");`
    - Upload: `west upload_fs --type lfs`
- `filename.bin` put into `S` goes into a basic readonly filesystem into one other partition of external flash.
    - Usage: `lv_img_set_src(img, "S:filename.bin");`
    - Upload: `west upload_fs --type raw`

## Which one to use?
Please use littlefs for now those options are mostly for experimentation. Using littlefs may be faster due to littlefs caching. However the other custom filesystem allows us to do more optimization for ZSWatch in the future. 