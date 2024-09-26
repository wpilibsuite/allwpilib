// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Pipe.h"

#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <wpi/SmallString.h>

namespace wpi::uv {

std::shared_ptr<Pipe> Pipe::Create(Loop& loop, bool ipc) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Pipe>(private_init{});
  int err = uv_pipe_init(loop.GetRaw(), h->GetRaw(), ipc ? 1 : 0);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Pipe::Reuse(std::function<void()> callback, bool ipc) {
  if (IsLoopClosing() || IsClosing()) {
    return;
  }
  if (!m_reuseData) {
    m_reuseData = std::make_unique<ReuseData>();
  }
  m_reuseData->callback = std::move(callback);
  m_reuseData->ipc = ipc;
  uv_close(GetRawHandle(), [](uv_handle_t* handle) {
    Pipe& h = *static_cast<Pipe*>(handle->data);
    if (!h.m_reuseData) {
      return;
    }
    auto data = std::move(h.m_reuseData);
    auto err =
        uv_pipe_init(h.GetLoopRef().GetRaw(), h.GetRaw(), data->ipc ? 1 : 0);
    if (err < 0) {
      h.ReportError(err);
      return;
    }
    data->callback();
  });
}

std::shared_ptr<Pipe> Pipe::Accept() {
  auto client = Create(GetLoopRef(), GetRaw()->ipc);
  if (!client) {
    return nullptr;
  }
  if (!Accept(client)) {
    client->Release();
    return nullptr;
  }
  return client;
}

Pipe* Pipe::DoAccept() {
  return Accept().get();
}

void Pipe::Bind(std::string_view name) {
  SmallString<128> nameBuf{name};
  Invoke(&uv_pipe_bind, GetRaw(), nameBuf.c_str());
}

void Pipe::Connect(std::string_view name,
                   const std::shared_ptr<PipeConnectReq>& req) {
  if (IsLoopClosing()) {
    return;
  }
  SmallString<128> nameBuf{name};
  uv_pipe_connect(req->GetRaw(), GetRaw(), nameBuf.c_str(),
                  [](uv_connect_t* req, int status) {
                    auto& h = *static_cast<PipeConnectReq*>(req->data);
                    if (status < 0) {
                      h.ReportError(status);
                    } else {
                      h.connected();
                    }
                    h.Release();  // this is always a one-shot
                  });
  req->Keep();
}

void Pipe::Connect(std::string_view name, std::function<void()> callback) {
  auto req = std::make_shared<PipeConnectReq>();
  req->connected.connect(std::move(callback));
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

}  // namespace wpi::uv
