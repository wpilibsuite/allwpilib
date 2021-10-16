#ifndef CONFIG_H
#define CONFIG_H

// Include this header only under the llvm source tree.
// This is a private header.

/* Exported configuration */
#include "wpi/llvm-config.h"

/* Bug report URL. */
#define BUG_REPORT_URL ""

/* Define to 1 to enable backtraces, and to 0 otherwise. */
#define ENABLE_BACKTRACES 0

/* Define to 1 to enable crash overrides, and to 0 otherwise. */
#define ENABLE_CRASH_OVERRIDES 0

/* Define to 1 to enable crash memory dumps, and to 0 otherwise. */
#define LLVM_ENABLE_CRASH_DUMPS 0

/* Define to 1 if you have the `backtrace' function. */
#define HAVE_BACKTRACE 0

#define BACKTRACE_HEADER <backtrace.h>

/* Define to 1 if you have the <CrashReporterClient.h> header file. */
#undef HAVE_CRASHREPORTERCLIENT_H

/* can use __crashreporter_info__ */
#define HAVE_CRASHREPORTER_INFO 0

/* Define to 1 if you have the declaration of `arc4random', and to 0 if you
   don't. */
#define HAVE_DECL_ARC4RANDOM 0

/* Define to 1 if you have the declaration of `FE_ALL_EXCEPT', and to 0 if you
   don't. */
#define HAVE_DECL_FE_ALL_EXCEPT 0

/* Define to 1 if you have the declaration of `FE_INEXACT', and to 0 if you
   don't. */
#define HAVE_DECL_FE_INEXACT 0

/* Define to 1 if you have the declaration of `strerror_s', and to 0 if you
   don't. */
#define HAVE_DECL_STRERROR_S 0

/* Define to 1 if you have the DIA SDK installed, and to 0 if you don't. */
#define LLVM_ENABLE_DIA_SDK 0

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H __has_include(<dlfcn.h>)

/* Define if dlopen() is available on this platform. */
#define HAVE_DLOPEN __has_include(<dlfcn.h>)

/* Define if dladdr() is available on this platform. */
#define HAVE_DLADDR __has_include(<dlfcn.h>)

/* Define to 1 if we can register EH frames on this platform. */
#define HAVE_REGISTER_FRAME 1

/* Define to 1 if we can deregister EH frames on this platform. */
#define HAVE_DEREGISTER_FRAME 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H __has_include(<errno.h>)

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H __has_include(<fcntl.h>)

/* Define to 1 if you have the <fenv.h> header file. */
#define HAVE_FENV_H __has_include(<fenv.h>)

/* Define if libffi is available on this platform. */
#define HAVE_FFI_CALL __has_include(<ffi.h>)

/* Define to 1 if you have the <ffi/ffi.h> header file. */
#define HAVE_FFI_FFI_H __has_include(<ffi/ffi.h>)

/* Define to 1 if you have the <ffi.h> header file. */
#define HAVE_FFI_H __has_include(<ffi.h>)

/* Define to 1 if you have the `futimens' function. */
#define HAVE_FUTIMENS __has_include(<sys/time.h>)

/* Define to 1 if you have the `futimes' function. */
#define HAVE_FUTIMES __has_include(<sys/time.h>)

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE __has_include(<unistd.h>)

/* Define to 1 if you have the `getrlimit' function. */
#define HAVE_GETRLIMIT __has_include(<sys/resource.h>)

/* Define to 1 if you have the `getrusage' function. */
#define HAVE_GETRUSAGE __has_include(<sys/resource.h>)

/* Define to 1 if you have the `isatty' function. */
#define HAVE_ISATTY 1

/* Define to 1 if you have the `edit' library (-ledit). */
#define HAVE_LIBEDIT 0

/* Define to 1 if you have the `pfm' library (-lpfm). */
#define HAVE_LIBPFM 0

/* Define to 1 if the `perf_branch_entry' struct has field cycles. */
#define LIBPFM_HAS_FIELD_CYCLES 0

/* Define to 1 if you have the `psapi' library (-lpsapi). */
#define HAVE_LIBPSAPI 0

/* Define to 1 if you have the `pthread' library (-lpthread). */
#define HAVE_LIBPTHREAD __has_include(<pthread.h>)

/* Define to 1 if you have the `pthread_getname_np' function. */
#define HAVE_PTHREAD_GETNAME_NP __has_include(<pthread.h>)

/* Define to 1 if you have the `pthread_setname_np' function. */
#define HAVE_PTHREAD_SETNAME_NP __has_include(<pthread.h>)

/* Define to 1 if you have the <link.h> header file. */
#define HAVE_LINK_H __has_include(<link.h>)

/* Define to 1 if you have the `lseek64' function. */
#define HAVE_LSEEK64 __has_include(<unistd.h>)

/* Define to 1 if you have the <mach/mach.h> header file. */
#define HAVE_MACH_MACH_H __has_include(<mach/mach.h>)

/* Define to 1 if you have the `mallctl' function. */
#define HAVE_MALLCTL __has_include(<malloc.h>)

/* Define to 1 if you have the `mallinfo' function. */
#define HAVE_MALLINFO __has_include(<malloc.h>)

/* Define to 1 if you have the `mallinfo2' function. */
#define HAVE_MALLINFO2 __has_include(<malloc.h>)

/* Define to 1 if you have the <malloc/malloc.h> header file. */
#define HAVE_MALLOC_MALLOC_H __has_include(<malloc/malloc.h>)

/* Define to 1 if you have the `malloc_zone_statistics' function. */
#define HAVE_MALLOC_ZONE_STATISTICS 0

/* Define to 1 if you have the `posix_fallocate' function. */
#define HAVE_POSIX_FALLOCATE __has_include(<fcntl.h>)

/* Define to 1 if you have the `posix_spawn' function. */
#define HAVE_POSIX_SPAWN __has_include(<spawn.h>)

/* Define to 1 if you have the `pread' function. */
#define HAVE_PREAD __has_include(<unistd.h>)

/* Have pthread_getspecific */
#define HAVE_PTHREAD_GETSPECIFIC __has_include(<pthread.h>)

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H __has_include(<pthread.h>)

/* Have pthread_mutex_lock */
#define HAVE_PTHREAD_MUTEX_LOCK __has_include(<pthread.h>)

/* Have pthread_rwlock_init */
#define HAVE_PTHREAD_RWLOCK_INIT __has_include(<pthread.h>)

/* Define to 1 if you have the `sbrk' function. */
#define HAVE_SBRK __has_include(<unistd.h>)

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV __has_include(<stdlib.h>)

/* Define to 1 if you have the `setrlimit' function. */
#define HAVE_SETRLIMIT __has_include(<sys/resource.h>)

/* Define to 1 if you have the `sigaltstack' function. */
#define HAVE_SIGALTSTACK __has_include(<signal.h>)

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H __has_include(<signal.h>)

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR __has_include(<string.h>)

/* Define to 1 if you have the `strerror_r' function. */
#define HAVE_STRERROR_R __has_include(<string.h>)

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF __has_include(<unistd.h>)

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H __has_include(<sys/ioctl.h>)

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H __has_include(<sys/mman.h>)

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H __has_include(<sys/param.h>)

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H __has_include(<sys/resource.h>)

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H __has_include(<sys/stat.h>)

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H __has_include(<sys/time.h>)

/* Define to 1 if stat struct has st_mtimespec member .*/
#define HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC 1

/* Define to 1 if stat struct has st_mtim member. */
#define HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H __has_include(<sys/types.h>)

/* Define if the setupterm() function is supported this platform. */
#define LLVM_ENABLE_TERMINFO __has_include(<curses.h>)

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H __has_include(<termios.h>)

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H __has_include(<unistd.h>)

/* Define to 1 if you have the <valgrind/valgrind.h> header file. */
#define HAVE_VALGRIND_VALGRIND_H __has_include(<valgrind/valgrind.h>)

/* Have host's _alloca */
#define HAVE__ALLOCA 0

/* Define to 1 if you have the `_chsize_s' function. */
#define HAVE__CHSIZE_S 0

/* Define to 1 if you have the `_Unwind_Backtrace' function. */
#define HAVE__UNWIND_BACKTRACE 0

/* Have host's __alloca */
#define HAVE___ALLOCA 0

/* Have host's __ashldi3 */
#define HAVE___ASHLDI3 0

/* Have host's __ashrdi3 */
#define HAVE___ASHRDI3 0

/* Have host's __chkstk */
#define HAVE___CHKSTK 0

/* Have host's __chkstk_ms */
#define HAVE___CHKSTK_MS 0

/* Have host's __cmpdi2 */
#define HAVE___CMPDI2 0

/* Have host's __divdi3 */
#define HAVE___DIVDI3 0

/* Have host's __fixdfdi */
#define HAVE___FIXDFDI 0

/* Have host's __fixsfdi */
#define HAVE___FIXSFDI 0

/* Have host's __floatdidf */
#define HAVE___FLOATDIDF 0

/* Have host's __lshrdi3 */
#define HAVE___LSHRDI3 0

/* Have host's __main */
#define HAVE___MAIN 0

/* Have host's __moddi3 */
#define HAVE___MODDI3 0

/* Have host's __udivdi3 */
#define HAVE___UDIVDI3 0

/* Have host's __umoddi3 */
#define HAVE___UMODDI3 0

/* Have host's ___chkstk */
#define HAVE____CHKSTK 0

/* Have host's ___chkstk_ms */
#define HAVE____CHKSTK_MS 0

/* Linker version detected at compile time. */
#define HOST_LINK_VERSION ""

/* Target triple LLVM will generate code for by default */
/* Doesn't use `cmakedefine` because it is allowed to be empty. */
#define LLVM_DEFAULT_TARGET_TRIPLE ""

/* Define if zlib compression is available */
#define LLVM_ENABLE_ZLIB 0

/* Define if overriding target triple is enabled */
#define LLVM_TARGET_TRIPLE_ENV ""

/* LLVM version information */
#define LLVM_VERSION_INFO "13.0.0"

/* Whether tools show host and target info when invoked with --version */
#define LLVM_VERSION_PRINTER_SHOW_HOST_TARGET_INFO 0

/* Define if libxml2 is supported on this platform. */
#define LLVM_ENABLE_LIBXML2 0

/* Define to the extension used for shared libraries, say, ".so". */
#define LTDL_SHLIB_EXT ".so"

/* Define to the extension used for plugin libraries, say, ".so". */
#define LLVM_PLUGIN_EXT ".so"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "13.0.0"

/* Define to the vendor of this package. */
#define PACKAGE_VENDOR "WPILib"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define if std::is_trivially_copyable is supported */
#define HAVE_STD_IS_TRIVIALLY_COPYABLE 1

/* Define to a function implementing stricmp */
#define stricmp stricmp

/* Define to a function implementing strdup */
#define strdup strdup

/* Whether GlobalISel rule coverage is being collected */
#define LLVM_GISEL_COV_ENABLED 0

/* Define to the default GlobalISel coverage file prefix */
#define LLVM_GISEL_COV_PREFIX ""

#define HAVE_PROC_PID_RUSAGE 1

#endif

