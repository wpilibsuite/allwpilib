// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_PROCESS_H_
#define WPIUTIL_WPI_UV_PROCESS_H_

#include <uv.h>

#include <initializer_list>
#include <memory>
#include <string>

#include "wpi/ArrayRef.h"
#include "wpi/Signal.h"
#include "wpi/SmallVector.h"
#include "wpi/Twine.h"
#include "wpi/uv/Handle.h"

namespace wpi::uv {

class Loop;
class Pipe;

/**
 * Process handle.
 * Process handles will spawn a new process and allow the user to control it
 * and establish communication channels with it using streams.
 */
class Process final : public HandleImpl<Process, uv_process_t> {
  struct private_init {};

 public:
  explicit Process(const private_init&) {}
  ~Process() noexcept override = default;

  /**
   * Structure for Spawn() option temporaries.  This is a reference type
   * similar to StringRef, so if this value is stored outside of a temporary,
   * be careful about overwriting what it points to.
   */
  struct Option {
    enum Type {
      kNone,
      kArg,
      kEnv,
      kCwd,
      kUid,
      kGid,
      kSetFlags,
      kClearFlags,
      kStdioIgnore,
      kStdioInheritFd,
      kStdioInheritPipe,
      kStdioCreatePipe
    };

    Option() : m_type(kNone) {}

    /*implicit*/ Option(const char* arg) {  // NOLINT
      m_data.str = arg;
    }

    /*implicit*/ Option(const std::string& arg) {  // NOLINT
      m_data.str = arg.data();
    }

    /*implicit*/ Option(StringRef arg)  // NOLINT
        : m_strData(arg) {
      m_data.str = m_strData.c_str();
    }

    /*implicit*/ Option(const SmallVectorImpl<char>& arg)  // NOLINT
        : m_strData(arg.data(), arg.size()) {
      m_data.str = m_strData.c_str();
    }

    /*implicit*/ Option(const Twine& arg)  // NOLINT
        : m_strData(arg.str()) {
      m_data.str = m_strData.c_str();
    }

    explicit Option(Type type) : m_type(type) {}

    Type m_type = kArg;
    std::string m_strData;
    union {
      const char* str;
      uv_uid_t uid;
      uv_gid_t gid;
      unsigned int flags;
      struct {
        size_t index;
        union {
          int fd;
          Pipe* pipe;
        };
        unsigned int flags;
      } stdio;
    } m_data;
  };

  /**
   * Set environment variable for the subprocess.  If not set, the parent's
   * environment is used.
   * @param env environment variable
   */
  static Option Env(const Twine& env) {
    Option o(Option::kEnv);
    o.m_strData = env.str();
    o.m_data.str = o.m_strData.c_str();
    return o;
  }

  /**
   * Set the current working directory for the subprocess.
   * @param cwd current working directory
   */
  static Option Cwd(const Twine& cwd) {
    Option o(Option::kCwd);
    o.m_strData = cwd.str();
    o.m_data.str = o.m_strData.c_str();
    return o;
  }

  /**
   * Set the child process' user id.
   * @param uid user id
   */
  static Option Uid(uv_uid_t uid) {
    Option o(Option::kUid);
    o.m_data.uid = uid;
    return o;
  }

  /**
   * Set the child process' group id.
   * @param gid group id
   */
  static Option Gid(uv_gid_t gid) {
    Option o(Option::kGid);
    o.m_data.gid = gid;
    return o;
  }

  /**
   * Set spawn flags.
   * @param flags Bitmask values from uv_process_flags.
   */
  static Option SetFlags(unsigned int flags) {
    Option o(Option::kSetFlags);
    o.m_data.flags = flags;
    return o;
  }

  /**
   * Clear spawn flags.
   * @param flags Bitmask values from uv_process_flags.
   */
  static Option ClearFlags(unsigned int flags) {
    Option o(Option::kClearFlags);
    o.m_data.flags = flags;
    return o;
  }

  /**
   * Explicitly ignore a stdio.
   * @param index stdio index
   */
  static Option StdioIgnore(size_t index) {
    Option o(Option::kStdioIgnore);
    o.m_data.stdio.index = index;
    return o;
  }

  /**
   * Inherit a file descriptor from the parent process.
   * @param index stdio index
   * @param fd parent file descriptor
   */
  static Option StdioInherit(size_t index, int fd) {
    Option o(Option::kStdioInheritFd);
    o.m_data.stdio.index = index;
    o.m_data.stdio.fd = fd;
    return o;
  }

  /**
   * Inherit a pipe from the parent process.
   * @param index stdio index
   * @param pipe pipe
   */
  static Option StdioInherit(size_t index, Pipe& pipe) {
    Option o(Option::kStdioInheritPipe);
    o.m_data.stdio.index = index;
    o.m_data.stdio.pipe = &pipe;
    return o;
  }

  /**
   * Create a pipe between the child and the parent.
   * @param index stdio index
   * @param pipe pipe
   * @param flags Some combination of UV_READABLE_PIPE, UV_WRITABLE_PIPE, and
   *              UV_OVERLAPPED_PIPE (Windows only, ignored on Unix).
   */
  static Option StdioCreatePipe(size_t index, Pipe& pipe, unsigned int flags) {
    Option o(Option::kStdioCreatePipe);
    o.m_data.stdio.index = index;
    o.m_data.stdio.pipe = &pipe;
    o.m_data.stdio.flags = flags;
    return o;
  }

  /**
   * Disables inheritance for file descriptors / handles that this process
   * inherited from its parent.  The effect is that child processes spawned
   * by this process don't accidentally inherit these handles.
   *
   * It is recommended to call this function as early in your program as
   * possible, before the inherited file descriptors can be closed or
   * duplicated.
   */
  static void DisableStdioInheritance() { uv_disable_stdio_inheritance(); }

  /**
   * Starts a process.  If the process is not successfully spawned, an error
   * is generated on the loop and this function returns nullptr.
   *
   * Possible reasons for failing to spawn would include (but not be limited to)
   * the file to execute not existing, not having permissions to use the setuid
   * or setgid specified, or not having enough memory to allocate for the new
   * process.
   *
   * @param loop Loop object where this handle runs.
   * @param file Path pointing to the program to be executed
   * @param options Process options
   */
  static std::shared_ptr<Process> SpawnArray(Loop& loop, const Twine& file,
                                             ArrayRef<Option> options);

  static std::shared_ptr<Process> SpawnArray(
      Loop& loop, const Twine& file, std::initializer_list<Option> options) {
    return SpawnArray(loop, file, makeArrayRef(options.begin(), options.end()));
  }

  template <typename... Args>
  static std::shared_ptr<Process> Spawn(Loop& loop, const Twine& file,
                                        const Args&... options) {
    return SpawnArray(loop, file, {options...});
  }

  /**
   * Starts a process.  If the process is not successfully spawned, an error
   * is generated on the loop and this function returns nullptr.
   *
   * Possible reasons for failing to spawn would include (but not be limited to)
   * the file to execute not existing, not having permissions to use the setuid
   * or setgid specified, or not having enough memory to allocate for the new
   * process.
   *
   * @param loop Loop object where this handle runs.
   * @param file Path pointing to the program to be executed
   * @param options Process options
   */
  static std::shared_ptr<Process> SpawnArray(const std::shared_ptr<Loop>& loop,
                                             const Twine& file,
                                             ArrayRef<Option> options) {
    return SpawnArray(*loop, file, options);
  }

  static std::shared_ptr<Process> SpawnArray(
      const std::shared_ptr<Loop>& loop, const Twine& file,
      std::initializer_list<Option> options) {
    return SpawnArray(*loop, file, options);
  }

  template <typename... Args>
  static std::shared_ptr<Process> Spawn(const std::shared_ptr<Loop>& loop,
                                        const Twine& file,
                                        const Args&... options) {
    return SpawnArray(*loop, file, {options...});
  }

  /**
   * Sends the specified signal to the process.
   * @param signum signal number
   */
  void Kill(int signum) { Invoke(&uv_process_kill, GetRaw(), signum); }

  /**
   * Sends the specified signal to the given PID.
   * @param pid process ID
   * @param signum signal number
   * @return 0 on success, otherwise error code.
   */
  static int Kill(int pid, int signum) noexcept { return uv_kill(pid, signum); }

  /**
   * Get the process ID.
   * @return Process ID.
   */
  uv_pid_t GetPid() const noexcept { return GetRaw()->pid; }

  /**
   * Signal generated when the process exits.  The parameters are the exit
   * status and the signal that caused the process to terminate, if any.
   */
  sig::Signal<int64_t, int> exited;
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_PROCESS_H_
