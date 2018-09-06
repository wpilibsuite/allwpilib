/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Tcp.h"

#include <cstring>

#include "wpi/uv/util.h"

namespace wpi {
namespace uv {

std::shared_ptr<Tcp> Tcp::Create(Loop& loop, unsigned int flags) {
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
  if (IsClosing()) return;
  if (!m_reuseData) m_reuseData = std::make_unique<ReuseData>();
  m_reuseData->callback = callback;
  m_reuseData->flags = flags;
  uv_close(GetRawHandle(), [](uv_handle_t* handle) {
    Tcp& h = *static_cast<Tcp*>(handle->data);
    if (!h.m_reuseData) return;  // just in case
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
  if (!client) return nullptr;
  if (!Accept(client)) {
    client->Release();
    return nullptr;
  }
  return client;
}

Tcp* Tcp::DoAccept() { return Accept().get(); }

void Tcp::Bind(const Twine& ip, unsigned int port, unsigned int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
}

void Tcp::Bind6(const Twine& ip, unsigned int port, unsigned int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
}

sockaddr_storage Tcp::GetSock() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_tcp_getsockname, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len))
    std::memset(&name, 0, sizeof(name));
  return name;
}

sockaddr_storage Tcp::GetPeer() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_tcp_getpeername, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len))
    std::memset(&name, 0, sizeof(name));
  return name;
}

void Tcp::Connect(const sockaddr& addr,
                  const std::shared_ptr<TcpConnectReq>& req) {
  if (Invoke(&uv_tcp_connect, req->GetRaw(), GetRaw(), &addr,
             [](uv_connect_t* req, int status) {
               auto& h = *static_cast<TcpConnectReq*>(req->data);
               if (status < 0)
                 h.ReportError(status);
               else
                 h.connected();
               h.Release();  // this is always a one-shot
             }))
    req->Keep();
}

void Tcp::Connect(const sockaddr& addr, std::function<void()> callback) {
  auto req = std::make_shared<TcpConnectReq>();
  req->connected.connect(callback);
  Connect(addr, req);
}

void Tcp::Connect(const Twine& ip, unsigned int port,
                  const std::shared_ptr<TcpConnectReq>& req) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Connect(reinterpret_cast<const sockaddr&>(addr), req);
}

void Tcp::Connect(const Twine& ip, unsigned int port,
                  std::function<void()> callback) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Connect(reinterpret_cast<const sockaddr&>(addr), callback);
}

void Tcp::Connect6(const Twine& ip, unsigned int port,
                   const std::shared_ptr<TcpConnectReq>& req) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Connect(reinterpret_cast<const sockaddr&>(addr), req);
}

void Tcp::Connect6(const Twine& ip, unsigned int port,
                   std::function<void()> callback) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Connect(reinterpret_cast<const sockaddr&>(addr), callback);
}

}  // namespace uv
}  // namespace wpi
