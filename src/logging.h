#ifndef LOGGING_H_
#define LOGGING_H_

#include <cstdio>
#include "ansi.h"

#define LOG_STD_PREFIX  " %s:%d: "
#define LOG_STD_POSTFIX "\n", __FILE__, __LINE__

#if 0
# define LOG_COLOR( code, color ) C_##color code C_RESET
#else
# define LOG_COLOR( code, color ) code
#endif



#define LOG_INFO_FILE stderr
#define LOG_INFO_PREFIX  LOG_COLOR( "I", BLUE ) LOG_STD_PREFIX
#define LOG_INFO_POSTFIX LOG_STD_POSTFIX

#define LOG_WARNING_FILE stderr
#define LOG_WARNING_PREFIX LOG_COLOR( "W", YELLOW ) LOG_STD_PREFIX
#define LOG_WARNING_POSTFIX LOG_STD_POSTFIX

#define LOG_ERROR_FILE stderr
#define LOG_ERROR_PREFIX LOG_COLOR( "E", RED ) LOG_STD_PREFIX
#define LOG_ERROR_POSTFIX LOG_STD_POSTFIX

#define LOG_INFO( fmt, ... )    fprintf( LOG_INFO_FILE, LOG_INFO_PREFIX fmt LOG_INFO_POSTFIX, ##__VA_ARGS__ )
#define LOG_WARNING( fmt, ... ) fprintf( LOG_WARNING_FILE, LOG_WARNING_PREFIX fmt LOG_WARNING_POSTFIX, ##__VA_ARGS__ )
#define LOG_ERROR( fmt, ... )   fprintf( LOG_ERROR_FILE, LOG_ERROR_PREFIX fmt LOG_ERROR_POSTFIX, ##__VA_ARGS__ )


#endif
