// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Tcp.h"

#include <cstring>
#include <functional>
#include <memory>
#include <utility>

#include "wpinet/uv/util.h"

namespace wpi::uv {

std::shared_ptr<Tcp> Tcp::Create(Loop& loop, unsigned int flags) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Tcp>(private_init{});
  int err = uv_tcp_init_ex(loop.GetRaw(), h->GetRaw(), flags);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Tcp::Reuse(std::function<void()> callback, unsigned int flags) {
  if (IsLoopClosing() || IsClosing()) {
    return;
  }
  if (!m_reuseData) {
    m_reuseData = std::make_unique<ReuseData>();
  }
  m_reuseData->callback = std::move(callback);
  m_reuseData->flags = flags;
  uv_close(GetRawHandle(), [](uv_handle_t* handle) {
    Tcp& h = *static_cast<Tcp*>(handle->data);
    if (!h.m_reuseData) {
      return;  // just in case
    }
    auto data = std::move(h.m_reuseData);
    int err = uv_tcp_init_ex(h.GetLoopRef().GetRaw(), h.GetRaw(), data->flags);
    if (err < 0) {
      h.ReportError(err);
      return;
    }
    data->callback();
  });
}

std::shared_ptr<Tcp> Tcp::Accept() {
  auto client = Create(GetLoopRef());
  if (!client) {
    return nullptr;
  }
  if (!Accept(client)) {
    client->Release();
    return nullptr;
  }
  return client;
}

Tcp* Tcp::DoAccept() {
  return Accept().get();
}

void Tcp::Bind(std::string_view ip, unsigned int port, unsigned int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

void Tcp::Bind6(std::string_view ip, unsigned int port, unsigned int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

sockaddr_storage Tcp::GetSock() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_tcp_getsockname, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len)) {
    std::memset(&name, 0, sizeof(name));
  }
  return name;
}

sockaddr_storage Tcp::GetPeer() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_tcp_getpeername, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len)) {
    std::memset(&name, 0, sizeof(name));
  }
  return name;
}

void Tcp::Connect(const sockaddr& addr,
                  const std::shared_ptr<TcpConnectReq>& req) {
  if (IsLoopClosing()) {
    return;
  }
  if (Invoke(&uv_tcp_connect, req->GetRaw(), GetRaw(), &addr,
             [](uv_connect_t* req, int status) {
               auto& h = *static_cast<TcpConnectReq*>(req->data);
               if (status < 0) {
                 h.ReportError(status);
               } else {
                 h.connected();
               }
               h.Release();  // this is always a one-shot
             })) {
    req->Keep();
  }
}

void Tcp::Connect(const sockaddr& addr, std::function<void()> callback) {
  if (IsLoopClosing()) {
    return;
  }
  auto req = std::make_shared<TcpConnectReq>();
  req->connected.connect(std::move(callback));
  Connect(addr, req);
}

void Tcp::Connect(std::string_view ip, unsigned int port,
                  const std::shared_ptr<TcpConnectReq>& req) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Connect(reinterpret_cast<const sockaddr&>(addr), req);
  }
}

void Tcp::Connect(std::string_view ip, unsigned int port,
                  std::function<void()> callback) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Connect(reinterpret_cast<const sockaddr&>(addr), std::move(callback));
  }
}

void Tcp::Connect6(std::string_view ip, unsigned int port,
                   const std::shared_ptr<TcpConnectReq>& req) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Connect(reinterpret_cast<const sockaddr&>(addr), req);
  }
}

void Tcp::Connect6(std::string_view ip, unsigned int port,
                   std::function<void()> callback) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Connect(reinterpret_cast<const sockaddr&>(addr), std::move(callback));
  }
}

void Tcp::CloseReset() {
  if (!IsClosing()) {
    uv_tcp_close_reset(GetRaw(), [](uv_handle_t* handle) {
      Tcp& h = *static_cast<Tcp*>(handle->data);
      h.closed();
      h.Release();  // free ourselves
    });
    ForceClosed();
  }
}

}  // namespace wpi::uv
