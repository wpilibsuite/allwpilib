// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/FsEvent.h"

#include <cstdlib>
#include <memory>
#include <string>

#include <wpi/SmallString.h>

#include "wpinet/uv/Loop.h"

namespace wpi::uv {

std::shared_ptr<FsEvent> FsEvent::Create(Loop& loop) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<FsEvent>(private_init{});
  int err = uv_fs_event_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void FsEvent::Start(std::string_view path, unsigned int flags) {
  SmallString<128> pathBuf{path};
  Invoke(
      &uv_fs_event_start, GetRaw(),
      [](uv_fs_event_t* handle, const char* filename, int events, int status) {
        FsEvent& h = *static_cast<FsEvent*>(handle->data);
        if (status < 0) {
          h.ReportError(status);
        } else {
          h.fsEvent(filename, events);
        }
      },
      pathBuf.c_str(), flags);
}

std::string FsEvent::GetPath() {
  // Per the libuv docs, GetPath() always gives us a null-terminated string.
  // common case should be small
  char buf[128];
  size_t size = 128;
  int r = uv_fs_event_getpath(GetRaw(), buf, &size);
  if (r == 0) {
    return buf;
  } else if (r == UV_ENOBUFS) {
    // need to allocate a big enough buffer
    char* buf2 = static_cast<char*>(std::malloc(size));
    r = uv_fs_event_getpath(GetRaw(), buf2, &size);
    if (r == 0) {
      std::string out{buf2};
      std::free(buf2);
      return out;
    }
    std::free(buf2);
  }
  ReportError(r);
  return std::string{};
}

}  // namespace wpi::uv
