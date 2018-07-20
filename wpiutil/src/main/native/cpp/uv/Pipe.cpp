/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Pipe.h"

#include <cstdlib>

#include "wpi/SmallString.h"

namespace wpi {
namespace uv {

std::shared_ptr<Pipe> Pipe::Create(Loop& loop, bool ipc) {
  auto h = std::make_shared<Pipe>(private_init{});
  int err = uv_pipe_init(loop.GetRaw(), h->GetRaw(), ipc ? 1 : 0);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

std::shared_ptr<Pipe> Pipe::Accept() {
  auto client = Create(GetLoopRef());
  if (!client) return nullptr;
  if (!Accept(client)) {
    client->Release();
    return nullptr;
  }
  return client;
}

Pipe* Pipe::DoAccept() { return Accept().get(); }

void Pipe::Bind(const Twine& name) {
  SmallString<128> nameBuf;
  Invoke(&uv_pipe_bind, GetRaw(),
         name.toNullTerminatedStringRef(nameBuf).data());
}

void Pipe::Connect(const Twine& name,
                   const std::shared_ptr<PipeConnectReq>& req) {
  SmallString<128> nameBuf;
  uv_pipe_connect(req->GetRaw(), GetRaw(),
                  name.toNullTerminatedStringRef(nameBuf).data(),
                  [](uv_connect_t* req, int status) {
                    auto& h = *static_cast<PipeConnectReq*>(req->data);
                    if (status < 0)
                      h.ReportError(status);
                    else
                      h.connected();
                    h.Release();  // this is always a one-shot
                  });
  req->Keep();
}

void Pipe::Connect(const Twine& name, std::function<void()> callback) {
  auto req = std::make_shared<PipeConnectReq>();
  req->connected.connect(callback);
  Connect(name, req);
}

std::string Pipe::GetSock() {
  // Per libuv docs, the returned buffer is NOT null terminated.
  // common case should be small
  char buf[128];
  size_t size = 128;
  int r = uv_pipe_getsockname(GetRaw(), buf, &size);
  if (r == 0) {
    return std::string{buf, size};
  } else if (r == UV_ENOBUFS) {
    // need to allocate a big enough buffer
    char* buf2 = static_cast<char*>(std::malloc(size));
    r = uv_pipe_getsockname(GetRaw(), buf2, &size);
    if (r == 0) {
      std::string out{buf2, size};
      std::free(buf2);
      return out;
    }
    std::free(buf2);
  }
  ReportError(r);
  return std::string{};
}

std::string Pipe::GetPeer() {
  // Per libuv docs, the returned buffer is NOT null terminated.
  // common case should be small
  char buf[128];
  size_t size = 128;
  int r = uv_pipe_getpeername(GetRaw(), buf, &size);
  if (r == 0) {
    return std::string{buf, size};
  } else if (r == UV_ENOBUFS) {
    // need to allocate a big enough buffer
    char* buf2 = static_cast<char*>(std::malloc(size));
    r = uv_pipe_getpeername(GetRaw(), buf2, &size);
    if (r == 0) {
      std::string out{buf2, size};
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
