/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Process.h"

#include "wpi/SmallString.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Pipe.h"

namespace wpi {
namespace uv {

std::shared_ptr<Process> Process::SpawnArray(Loop& loop, const Twine& file,
                                             ArrayRef<Option> options) {
  // convert Option array to libuv structure
  uv_process_options_t coptions;

  coptions.exit_cb = [](uv_process_t* handle, int64_t status, int signal) {
    auto& h = *static_cast<Process*>(handle->data);
    h.exited(status, signal);
  };

  SmallString<128> fileBuf;
  coptions.file = file.toNullTerminatedStringRef(fileBuf).data();
  coptions.cwd = nullptr;
  coptions.flags = 0;
  coptions.uid = 0;
  coptions.gid = 0;

  SmallVector<char*, 4> argsBuf;
  SmallVector<char*, 4> envBuf;
  struct StdioContainer : public uv_stdio_container_t {
    StdioContainer() {
      flags = UV_IGNORE;
      data.fd = 0;
    }
  };
  SmallVector<StdioContainer, 4> stdioBuf;

  for (auto&& o : options) {
    switch (o.m_type) {
      case Option::kArg:
        argsBuf.push_back(const_cast<char*>(o.m_data.str));
        break;
      case Option::kEnv:
        envBuf.push_back(const_cast<char*>(o.m_data.str));
        break;
      case Option::kCwd:
        coptions.cwd = o.m_data.str[0] == '\0' ? nullptr : o.m_data.str;
        break;
      case Option::kUid:
        coptions.uid = o.m_data.uid;
        coptions.flags |= UV_PROCESS_SETUID;
        break;
      case Option::kGid:
        coptions.gid = o.m_data.gid;
        coptions.flags |= UV_PROCESS_SETGID;
        break;
      case Option::kSetFlags:
        coptions.flags |= o.m_data.flags;
        break;
      case Option::kClearFlags:
        coptions.flags &= ~o.m_data.flags;
        break;
      case Option::kStdioIgnore: {
        size_t index = o.m_data.stdio.index;
        if (index >= stdioBuf.size()) stdioBuf.resize(index + 1);
        stdioBuf[index].flags = UV_IGNORE;
        stdioBuf[index].data.fd = 0;
        break;
      }
      case Option::kStdioInheritFd: {
        size_t index = o.m_data.stdio.index;
        if (index >= stdioBuf.size()) stdioBuf.resize(index + 1);
        stdioBuf[index].flags = UV_INHERIT_FD;
        stdioBuf[index].data.fd = o.m_data.stdio.fd;
        break;
      }
      case Option::kStdioInheritPipe: {
        size_t index = o.m_data.stdio.index;
        if (index >= stdioBuf.size()) stdioBuf.resize(index + 1);
        stdioBuf[index].flags = UV_INHERIT_STREAM;
        stdioBuf[index].data.stream = o.m_data.stdio.pipe->GetRawStream();
        break;
      }
      case Option::kStdioCreatePipe: {
        size_t index = o.m_data.stdio.index;
        if (index >= stdioBuf.size()) stdioBuf.resize(index + 1);
        stdioBuf[index].flags =
            static_cast<uv_stdio_flags>(UV_CREATE_PIPE | o.m_data.stdio.flags);
        stdioBuf[index].data.stream = o.m_data.stdio.pipe->GetRawStream();
        break;
      }
      default:
        break;
    }
  }

  if (argsBuf.empty()) argsBuf.push_back(const_cast<char*>(coptions.file));
  argsBuf.push_back(nullptr);
  coptions.args = argsBuf.data();

  if (envBuf.empty()) {
    coptions.env = nullptr;
  } else {
    envBuf.push_back(nullptr);
    coptions.env = envBuf.data();
  }

  coptions.stdio_count = stdioBuf.size();
  coptions.stdio = static_cast<uv_stdio_container_t*>(stdioBuf.data());

  auto h = std::make_shared<Process>(private_init{});
  int err = uv_spawn(loop.GetRaw(), h->GetRaw(), &coptions);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

}  // namespace uv
}  // namespace wpi
