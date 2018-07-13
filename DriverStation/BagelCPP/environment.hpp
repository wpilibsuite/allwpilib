#pragma once

#include <string>
#include "library.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define OS_WIN
#endif
#if defined(__APPLE__)
    #define OS_MAC
    #define OS_UNIX
#endif
#if defined(__linux) || defined(__linux__)
    #define OS_LINUX
#endif
#if defined(__unix) || defined(__unix__)
    #define OS_UNIX
#endif

#if defined(OS_UNIX)
    #include <unistd.h>
#endif

#if defined(__posix) || defined(_POSIX_VERSION)
    #define OS_POSIX
#endif

#include <stdlib.h>

namespace Toast {
    namespace Environment {

        namespace OS {
            API bool is_windows();
            API bool is_mac();
            API bool is_linux();
            
            API bool is_unix();
            API bool is_posix();
            
            API std::string to_string();
        }
        
        API void put_env(char *string);
        API char *get_env(char *name);
        
        API std::string toast_home();
		API std::string user_home();
    }
}