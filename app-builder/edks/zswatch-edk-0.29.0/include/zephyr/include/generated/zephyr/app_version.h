#ifndef _APP_VERSION_H_
#define _APP_VERSION_H_

/* The template values come from cmake/modules/version.cmake
 * BUILD_VERSION related template values will be 'git describe',
 * alternatively user defined BUILD_VERSION.
 */

/* #undef ZEPHYR_VERSION_CODE */
/* #undef ZEPHYR_VERSION */

#define APPVERSION                   0x80000
#define APP_VERSION_NUMBER           0x800
#define APP_VERSION_MAJOR            0
#define APP_VERSION_MINOR            8
#define APP_PATCHLEVEL               0
#define APP_TWEAK                    0
#define APP_VERSION_STRING           "0.8.0"
#define APP_VERSION_EXTENDED_STRING  "0.8.0+0"
#define APP_VERSION_TWEAK_STRING     "0.8.0+0"

#define APP_BUILD_VERSION 1ab053dc529e


#endif /* _APP_VERSION_H_ */
