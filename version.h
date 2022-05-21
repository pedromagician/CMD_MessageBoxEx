#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VERSION_MAJOR               0
#define VERSION_MINOR               5
#define VERSION_PATCH               8
#define VERSION_REVISION            0

#define VER_COMPANYNAME_STR         "Pedro"
#define VER_FILE_DESCRIPTION_STR    "MessageBoxEx for command line. Amiga Rulez!"

#define VER_PRODUCTNAME_STR         "MessageBoxEx"
#define VER_INTERNAL_NAME_STR       VER_PRODUCTNAME_STR
#define VER_ORIGINAL_FILENAME_STR   VER_PRODUCTNAME_STR ".exe"

#define VER_COPYRIGHT_STR           "Copyright (C) 2022"

#define VER_FILE_VERSION_STR        STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH) "." STRINGIZE(VERSION_REVISION)
#define VER_PRODUCT_VERSION_STR     VER_FILE_VERSION_STR
#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_REVISION
#define VER_PRODUCT_VERSION         VER_FILE_VERSION
