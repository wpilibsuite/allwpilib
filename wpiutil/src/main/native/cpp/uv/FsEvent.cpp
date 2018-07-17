/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/FsEvent.h"

#include <cstdlib>

#include "wpi/SmallString.h"
#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

std::shared_ptr<FsEvent> FsEvent::Create(Loop& loop) {
  auto h = std::make_shared<FsEvent>(private_init{});
  int err = uv_fs_event_init(loop.GetRaw(), h->GetRaw());
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void FsEvent::Start(const Twine& path, unsigned int flags) {
  SmallString<128> pathBuf;
  Invoke(
      &uv_fs_event_start, GetRaw(),
      [](uv_fs_event_t* handle, const char* filename, int events, int status) {
        FsEvent& h = *static_cast<FsEvent*>(handle->data);
        if (status < 0)
          h.ReportError(status);
        else
          h.fsEvent(filename, events);
      },
      path.toNullTerminatedStringRef(pathBuf).data(), flags);
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

}  // namespace uv
}  // namespace wpi
