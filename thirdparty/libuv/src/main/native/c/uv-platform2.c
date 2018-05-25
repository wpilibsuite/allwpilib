#ifdef __APPLE__
#include "unix/darwin-proctitle.c"
#elif defined(_WIN32)
#include "win/pipe.c"
#endif

void uv_platform2_anchor(void) {}
