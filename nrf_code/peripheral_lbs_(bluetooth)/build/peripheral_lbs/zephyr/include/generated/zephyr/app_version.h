#ifndef _APP_VERSION_H_
#define _APP_VERSION_H_

/* The template values come from cmake/modules/version.cmake
 * BUILD_VERSION related template values will be 'git describe',
 * alternatively user defined BUILD_VERSION.
 */

/* #undef ZEPHYR_VERSION_CODE */
/* #undef ZEPHYR_VERSION */

#define APPVERSION                   0x3020200
#define APP_VERSION_NUMBER           0x30202
#define APP_VERSION_MAJOR            3
#define APP_VERSION_MINOR            2
#define APP_PATCHLEVEL               2
#define APP_TWEAK                    0
#define APP_VERSION_STRING           "3.2.2"
#define APP_VERSION_EXTENDED_STRING  "3.2.2+0"
#define APP_VERSION_TWEAK_STRING     "3.2.2+0"

#define APP_BUILD_VERSION 749efea753f5


#endif /* _APP_VERSION_H_ */
