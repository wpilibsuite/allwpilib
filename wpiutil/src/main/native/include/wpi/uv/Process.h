/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_PROCESS_H_
#define WPIUTIL_WPI_UV_PROCESS_H_

#include <uv.h>

#include <memory>
#include <string>

#include "wpi/Signal.h"
#include "wpi/SmallVector.h"
#include "wpi/Twine.h"
#include "wpi/uv/Handle.h"

namespace wpi {
namespace uv {

class Loop;
class Pipe;

/**
 * Process options.
 */
class ProcessOptions final {
  friend class Process;

 public:
  /**
   * Set environment variables for the subprocess.  If not set or set to
   * nullptr, the parent's environment is used.
   * @param env environment variables
   */
  ProcessOptions& SetEnv(char** env) {
    m_env = env;
    return *this;
  }

  /**
   * Set the current working directory for the subprocess.
   * @param cwd current working directory
   */
  ProcessOptions& SetCwd(const Twine& cwd) {
    m_cwd = cwd.str();
    return *this;
  }

  /**
   * Set the child process' user id.
   * @param uid user id
   */
  ProcessOptions& SetUid(uv_uid_t uid) noexcept {
    m_uid = uid;
    m_flags |= UV_PROCESS_SETUID;
    return *this;
  }

  /**
   * Set the child process' group id.
   * @param gid group id
   */
  ProcessOptions& SetGid(uv_gid_t gid) noexcept {
    m_gid = gid;
    m_flags |= UV_PROCESS_SETGID;
    return *this;
  }

  /**
   * Set flags.
   * @param flags Bitmask values from uv_process_flags.
   */
  ProcessOptions& SetFlags(unsigned int flags) noexcept {
    m_flags |= flags;
    return *this;
  }

  /**
   * Clear flags.
   * @param flags Bitmask values from uv_process_flags.
   */
  ProcessOptions& ClearFlags(unsigned int flags) noexcept {
    m_flags &= ~flags;
    return *this;
  }

  /**
   * Explicitly ignore a stdio.
   * @param index stdio index
   */
  ProcessOptions& StdioIgnore(size_t index);

  /**
   * Inherit a file descriptor from the parent process.
   * @param index stdio index
   * @param fd parent file descriptor
   */
  ProcessOptions& StdioInherit(size_t index, int fd);

  /**
   * Inherit a pipe from the parent process.
   * @param index stdio index
   * @param pipe pipe
   */
  ProcessOptions& StdioInherit(size_t index, Pipe& pipe);

  /**
   * Create a pipe between the child and the parent.
   * @param index stdio index
   * @param pipe pipe
   * @param flags Some combination of UV_READABLE_PIPE, UV_WRITABLE_PIPE, and
   *              UV_OVERLAPPED_PIPE (Windows only, ignored on Unix).
   */
  ProcessOptions& StdioCreatePipe(size_t index, Pipe& pipe, unsigned int flags);

 private:
  char** m_env = nullptr;
  std::string m_cwd;
  unsigned int m_flags = 0;
  struct StdioContainer : public uv_stdio_container_t {
    StdioContainer() {
      flags = UV_IGNORE;
      data.fd = 0;
    }
  };
  SmallVector<StdioContainer, 4> m_stdio;
  uv_uid_t m_uid = 0;
  uv_gid_t m_gid = 0;
};

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
   * @param args Command line arguments
   * @param options Process options
   */
  static std::shared_ptr<Process> Spawn(
      Loop& loop, const Twine& file, char** args,
      const ProcessOptions& options = ProcessOptions{});

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
   * @param args Command line arguments
   * @param options Process options
   */
  static std::shared_ptr<Process> Spawn(
      const std::shared_ptr<Loop>& loop, const Twine& file, char** args,
      const ProcessOptions& options = ProcessOptions{}) {
    return Spawn(*loop, file, args, options);
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

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_PROCESS_H_
