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

ProcessOptions& ProcessOptions::StdioIgnore(size_t index) {
  if (index >= m_stdio.size()) m_stdio.resize(index + 1);
  m_stdio[index].flags = UV_IGNORE;
  m_stdio[index].data.fd = 0;
  return *this;
}

ProcessOptions& ProcessOptions::StdioInherit(size_t index, int fd) {
  if (index >= m_stdio.size()) m_stdio.resize(index + 1);
  m_stdio[index].flags = UV_INHERIT_FD;
  m_stdio[index].data.fd = fd;
  return *this;
}

ProcessOptions& ProcessOptions::StdioInherit(size_t index, Pipe& pipe) {
  if (index >= m_stdio.size()) m_stdio.resize(index + 1);
  m_stdio[index].flags = UV_INHERIT_STREAM;
  m_stdio[index].data.stream = pipe.GetRawStream();
  return *this;
}

ProcessOptions& ProcessOptions::StdioCreatePipe(size_t index, Pipe& pipe,
                                                unsigned int flags) {
  if (index >= m_stdio.size()) m_stdio.resize(index + 1);
  m_stdio[index].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | flags);
  m_stdio[index].data.stream = pipe.GetRawStream();
  return *this;
}

std::shared_ptr<Process> Process::Spawn(Loop& loop, const Twine& file,
                                        char** args,
                                        const ProcessOptions& options) {
  // convert ProcessOptions to libuv structure
  SmallString<128> fileBuf;
  uv_process_options_t coptions;
  coptions.exit_cb = [](uv_process_t* handle, int64_t status, int signal) {
    auto& h = *static_cast<Process*>(handle->data);
    h.exited(status, signal);
  };
  coptions.file = file.toNullTerminatedStringRef(fileBuf).data();
  coptions.args = args;
  coptions.env = options.m_env;
  coptions.cwd = options.m_cwd.empty() ? nullptr : options.m_cwd.c_str();
  coptions.flags = options.m_flags;
  coptions.stdio_count = options.m_stdio.size();
  coptions.stdio = const_cast<uv_stdio_container_t*>(
      static_cast<const uv_stdio_container_t*>(options.m_stdio.data()));
  coptions.uid = options.m_uid;
  coptions.gid = options.m_gid;

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
