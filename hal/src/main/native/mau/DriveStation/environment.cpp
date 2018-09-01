#include "include/environment.hpp"

#ifndef OS_WIN
#include <pwd.h>
#endif

using namespace Toast::Environment;
using namespace Toast;
using namespace std;

bool OS::is_windows() { 
    #ifdef OS_WIN
        return true;
    #else
        return false;
    #endif
}

bool OS::is_mac() {
    #ifdef OS_MAC
        return true;
    #else
        return false;
    #endif
}

bool OS::is_linux() {
    #ifdef OS_LINUX
        return true;
    #else
        return false;
    #endif
}

bool OS::is_unix() {
    #ifdef OS_UNIX
        return true;
    #else
        return false;
    #endif
}

bool OS::is_posix() {
    #ifdef OS_POSIX
        return true;
    #else
        return false;
    #endif
}

string OS::to_string() {
    string s = "";
    if (OS::is_windows()) s += "Windows ";
    if (OS::is_mac())     s += "Mac ";
    if (OS::is_linux())   s += "Linux ";
    if (OS::is_unix())    s += "(UNIX) ";
    if (OS::is_posix())   s += "(POSIX) ";
    return s.substr(0, s.length() - 1);;
}

void Environment::put_env(char *string) {
    putenv(string);
}

char *Environment::get_env(char *string) {
    return getenv(string);
}

string Environment::toast_home() {
	return "include/";
}

string Environment::user_home() {
	char *var = getenv("USER");
	if (var) {
		return string(var);
	} else {

	#ifdef OS_WIN
		var = getenv("USERPROFILE");
		if (var) {
			return string(var);
		} else {
			return "/";
		}
    #else
		return string(getpwuid(getuid())->pw_dir);
	#endif

	}
}