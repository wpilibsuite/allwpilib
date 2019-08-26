//===- lib/Support/ErrorHandling.cpp - Callbacks for errors ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an API used to indicate fatal error conditions.  Non-fatal
// errors (most of them) should be handled through LLVMContext.
//
//===----------------------------------------------------------------------===//

#include "wpi/ErrorHandling.h"
#include "wpi/SmallVector.h"
#include "wpi/Twine.h"
#include "wpi/Error.h"
#include "wpi/WindowsError.h"
#include "wpi/raw_ostream.h"
#include <cassert>
#include <cstdlib>
#include <mutex>
#include <new>

#ifndef _WIN32
#include <unistd.h>
#endif

#if defined(_MSC_VER)
#include <io.h>
#endif

using namespace wpi;

static fatal_error_handler_t ErrorHandler = nullptr;
static void *ErrorHandlerUserData = nullptr;

static fatal_error_handler_t BadAllocErrorHandler = nullptr;
static void *BadAllocErrorHandlerUserData = nullptr;

// Mutexes to synchronize installing error handlers and calling error handlers.
// Do not use ManagedStatic, or that may allocate memory while attempting to
// report an OOM.
//
// This usage of std::mutex has to be conditionalized behind ifdefs because
// of this script:
//   compiler-rt/lib/sanitizer_common/symbolizer/scripts/build_symbolizer.sh
// That script attempts to statically link the LLVM symbolizer library with the
// STL and hide all of its symbols with 'opt -internalize'. To reduce size, it
// cuts out the threading portions of the hermetic copy of libc++ that it
// builds. We can remove these ifdefs if that script goes away.
static std::mutex ErrorHandlerMutex;
static std::mutex BadAllocErrorHandlerMutex;

void wpi::install_fatal_error_handler(fatal_error_handler_t handler,
                                      void *user_data) {
  std::scoped_lock Lock(ErrorHandlerMutex);
  assert(!ErrorHandler && "Error handler already registered!\n");
  ErrorHandler = handler;
  ErrorHandlerUserData = user_data;
}

void wpi::remove_fatal_error_handler() {
  std::scoped_lock Lock(ErrorHandlerMutex);
  ErrorHandler = nullptr;
  ErrorHandlerUserData = nullptr;
}

void wpi::report_fatal_error(const char *Reason, bool GenCrashDiag) {
  report_fatal_error(Twine(Reason), GenCrashDiag);
}

void wpi::report_fatal_error(const std::string &Reason, bool GenCrashDiag) {
  report_fatal_error(Twine(Reason), GenCrashDiag);
}

void wpi::report_fatal_error(StringRef Reason, bool GenCrashDiag) {
  report_fatal_error(Twine(Reason), GenCrashDiag);
}

void wpi::report_fatal_error(const Twine &Reason, bool GenCrashDiag) {
  wpi::fatal_error_handler_t handler = nullptr;
  void* handlerData = nullptr;
  {
    // Only acquire the mutex while reading the handler, so as not to invoke a
    // user-supplied callback under a lock.
    std::scoped_lock Lock(ErrorHandlerMutex);
    handler = ErrorHandler;
    handlerData = ErrorHandlerUserData;
  }

  if (handler) {
    handler(handlerData, Reason.str(), GenCrashDiag);
  } else {
    // Blast the result out to stderr.  We don't try hard to make sure this
    // succeeds (e.g. handling EINTR) and we can't use errs() here because
    // raw ostreams can call report_fatal_error.
    SmallVector<char, 64> Buffer;
    raw_svector_ostream OS(Buffer);
    OS << "LLVM ERROR: " << Reason << "\n";
    StringRef MessageStr = OS.str();
#ifdef _WIN32
    int written = ::_write(2, MessageStr.data(), MessageStr.size());
#else
    ssize_t written = ::write(2, MessageStr.data(), MessageStr.size());
#endif
    (void)written; // If something went wrong, we deliberately just give up.
  }

  exit(1);
}

void wpi::install_bad_alloc_error_handler(fatal_error_handler_t handler,
                                          void *user_data) {
  std::scoped_lock Lock(BadAllocErrorHandlerMutex);
  assert(!ErrorHandler && "Bad alloc error handler already registered!\n");
  BadAllocErrorHandler = handler;
  BadAllocErrorHandlerUserData = user_data;
}

void wpi::remove_bad_alloc_error_handler() {
  std::scoped_lock Lock(BadAllocErrorHandlerMutex);
  BadAllocErrorHandler = nullptr;
  BadAllocErrorHandlerUserData = nullptr;
}

void wpi::report_bad_alloc_error(const char *Reason, bool GenCrashDiag) {
  fatal_error_handler_t Handler = nullptr;
  void *HandlerData = nullptr;
  {
    // Only acquire the mutex while reading the handler, so as not to invoke a
    // user-supplied callback under a lock.
    std::scoped_lock Lock(BadAllocErrorHandlerMutex);
    Handler = BadAllocErrorHandler;
    HandlerData = BadAllocErrorHandlerUserData;
  }

  if (Handler) {
    Handler(HandlerData, Reason, GenCrashDiag);
    wpi_unreachable("bad alloc handler should not return");
  }

  // Don't call the normal error handler. It may allocate memory. Directly write
  // an OOM to stderr and abort.
  char OOMMessage[] = "LLVM ERROR: out of memory\n";
#ifdef _WIN32
  int written = ::_write(2, OOMMessage, strlen(OOMMessage));
#else
  ssize_t written = ::write(2, OOMMessage, strlen(OOMMessage));
#endif
  (void)written;
  abort();
}

// Causes crash on allocation failure. It is called prior to the handler set by
// 'install_bad_alloc_error_handler'.
static void out_of_memory_new_handler() {
  wpi::report_bad_alloc_error("Allocation failed");
}

// Installs new handler that causes crash on allocation failure. It does not
// need to be called explicitly, if this file is linked to application, because
// in this case it is called during construction of 'new_handler_installer'.
void wpi::install_out_of_memory_new_handler() {
  static bool out_of_memory_new_handler_installed = false;
  if (!out_of_memory_new_handler_installed) {
    std::set_new_handler(out_of_memory_new_handler);
    out_of_memory_new_handler_installed = true;
  }
}

// Static object that causes installation of 'out_of_memory_new_handler' before
// execution of 'main'.
static class NewHandlerInstaller {
public:
  NewHandlerInstaller() {
    install_out_of_memory_new_handler();
  }
} new_handler_installer;

void wpi::wpi_unreachable_internal(const char *msg, const char *file,
                                   unsigned line) {
  // This code intentionally doesn't call the ErrorHandler callback, because
  // wpi_unreachable is intended to be used to indicate "impossible"
  // situations, and not legitimate runtime errors.
  if (msg)
    errs() << msg << "\n";
  errs() << "UNREACHABLE executed";
  if (file)
    errs() << " at " << file << ":" << line;
  errs() << "!\n";
  abort();
#ifdef LLVM_BUILTIN_UNREACHABLE
  // Windows systems and possibly others don't declare abort() to be noreturn,
  // so use the unreachable builtin to avoid a Clang self-host warning.
  LLVM_BUILTIN_UNREACHABLE;
#endif
}

#ifdef _WIN32

#include <system_error>
#include <winerror.h>

// I'd rather not double the line count of the following.
#define MAP_ERR_TO_COND(x, y)                                                  \
  case x:                                                                      \
    return std::make_error_code(std::errc::y)

std::error_code wpi::mapWindowsError(unsigned EV) {
  switch (EV) {
    MAP_ERR_TO_COND(ERROR_ACCESS_DENIED, permission_denied);
    MAP_ERR_TO_COND(ERROR_ALREADY_EXISTS, file_exists);
    MAP_ERR_TO_COND(ERROR_BAD_UNIT, no_such_device);
    MAP_ERR_TO_COND(ERROR_BUFFER_OVERFLOW, filename_too_long);
    MAP_ERR_TO_COND(ERROR_BUSY, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_BUSY_DRIVE, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_CANNOT_MAKE, permission_denied);
    MAP_ERR_TO_COND(ERROR_CANTOPEN, io_error);
    MAP_ERR_TO_COND(ERROR_CANTREAD, io_error);
    MAP_ERR_TO_COND(ERROR_CANTWRITE, io_error);
    MAP_ERR_TO_COND(ERROR_CURRENT_DIRECTORY, permission_denied);
    MAP_ERR_TO_COND(ERROR_DEV_NOT_EXIST, no_such_device);
    MAP_ERR_TO_COND(ERROR_DEVICE_IN_USE, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_DIR_NOT_EMPTY, directory_not_empty);
    MAP_ERR_TO_COND(ERROR_DIRECTORY, invalid_argument);
    MAP_ERR_TO_COND(ERROR_DISK_FULL, no_space_on_device);
    MAP_ERR_TO_COND(ERROR_FILE_EXISTS, file_exists);
    MAP_ERR_TO_COND(ERROR_FILE_NOT_FOUND, no_such_file_or_directory);
    MAP_ERR_TO_COND(ERROR_HANDLE_DISK_FULL, no_space_on_device);
    MAP_ERR_TO_COND(ERROR_INVALID_ACCESS, permission_denied);
    MAP_ERR_TO_COND(ERROR_INVALID_DRIVE, no_such_device);
    MAP_ERR_TO_COND(ERROR_INVALID_FUNCTION, function_not_supported);
    MAP_ERR_TO_COND(ERROR_INVALID_HANDLE, invalid_argument);
    MAP_ERR_TO_COND(ERROR_INVALID_NAME, invalid_argument);
    MAP_ERR_TO_COND(ERROR_LOCK_VIOLATION, no_lock_available);
    MAP_ERR_TO_COND(ERROR_LOCKED, no_lock_available);
    MAP_ERR_TO_COND(ERROR_NEGATIVE_SEEK, invalid_argument);
    MAP_ERR_TO_COND(ERROR_NOACCESS, permission_denied);
    MAP_ERR_TO_COND(ERROR_NOT_ENOUGH_MEMORY, not_enough_memory);
    MAP_ERR_TO_COND(ERROR_NOT_READY, resource_unavailable_try_again);
    MAP_ERR_TO_COND(ERROR_OPEN_FAILED, io_error);
    MAP_ERR_TO_COND(ERROR_OPEN_FILES, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_OUTOFMEMORY, not_enough_memory);
    MAP_ERR_TO_COND(ERROR_PATH_NOT_FOUND, no_such_file_or_directory);
    MAP_ERR_TO_COND(ERROR_BAD_NETPATH, no_such_file_or_directory);
    MAP_ERR_TO_COND(ERROR_READ_FAULT, io_error);
    MAP_ERR_TO_COND(ERROR_RETRY, resource_unavailable_try_again);
    MAP_ERR_TO_COND(ERROR_SEEK, io_error);
    MAP_ERR_TO_COND(ERROR_SHARING_VIOLATION, permission_denied);
    MAP_ERR_TO_COND(ERROR_TOO_MANY_OPEN_FILES, too_many_files_open);
    MAP_ERR_TO_COND(ERROR_WRITE_FAULT, io_error);
    MAP_ERR_TO_COND(ERROR_WRITE_PROTECT, permission_denied);
    MAP_ERR_TO_COND(WSAEACCES, permission_denied);
    MAP_ERR_TO_COND(WSAEBADF, bad_file_descriptor);
    MAP_ERR_TO_COND(WSAEFAULT, bad_address);
    MAP_ERR_TO_COND(WSAEINTR, interrupted);
    MAP_ERR_TO_COND(WSAEINVAL, invalid_argument);
    MAP_ERR_TO_COND(WSAEMFILE, too_many_files_open);
    MAP_ERR_TO_COND(WSAENAMETOOLONG, filename_too_long);
  default:
    return std::error_code(EV, std::system_category());
  }
}

#endif
