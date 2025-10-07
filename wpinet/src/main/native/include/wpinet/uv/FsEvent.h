// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_FSEVENT_H_
#define WPINET_UV_FSEVENT_H_

#include <uv.h>

#include <memory>
#include <string>
#include <string_view>

#include <wpi/Signal.h>

#include "wpinet/uv/Handle.h"

namespace wpi::uv {

class Loop;

/**
 * Filesystem Event handle.
 */
class FsEvent final : public HandleImpl<FsEvent, uv_fs_event_t> {
  struct private_init {};

 public:
  explicit FsEvent(const private_init&) {}
  ~FsEvent() noexcept override = default;

  /**
   * Create a filesystem event handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<FsEvent> Create(Loop& loop);

  /**
   * Create a filesystem event handle.
   *
   * @param loop Loop object where this handle runs.
   */
  static std::shared_ptr<FsEvent> Create(const std::shared_ptr<Loop>& loop) {
    return Create(*loop);
  }

  /**
   * Start watching the specified path for changes.
   *
   * @param path Path to watch for changes
   * @param flags Bitmask of event flags.  Only UV_FS_EVENT_RECURSIVE is
   *               supported (and only on OSX and Windows).
   */
  void Start(std::string_view path, unsigned int flags = 0);

  /**
   * Stop watching for changes.
   */
  void Stop() { Invoke(&uv_fs_event_stop, GetRaw()); }

  /**
   * Get the path being monitored.  Signals error and returns empty string if
   * an error occurs.
   * @return Monitored path.
   */
  std::string GetPath();

  /**
   * Signal generated when a filesystem change occurs.  The first parameter
   * is the filename (if a directory was passed to Start(), the filename is
   * relative to that directory).  The second parameter is an ORed mask of
   * UV_RENAME and UV_CHANGE.
   */
  sig::Signal<const char*, int> fsEvent;
};

}  // namespace wpi::uv

#endif  // WPINET_UV_FSEVENT_H_
