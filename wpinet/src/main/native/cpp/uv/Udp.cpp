// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Udp.h"

#include <cstring>
#include <functional>
#include <memory>
#include <utility>

#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>

#include "wpinet/uv/util.h"

namespace {

using namespace wpi;
using namespace wpi::uv;

class CallbackUdpSendReq : public UdpSendReq {
 public:
  CallbackUdpSendReq(std::span<const Buffer> bufs,
                     std::function<void(std::span<Buffer>, Error)> callback)
      : m_bufs{bufs.begin(), bufs.end()} {
    complete.connect(
        [this, f = std::move(callback)](Error err) { f(m_bufs, err); });
  }

 private:
  SmallVector<Buffer, 4> m_bufs;
};

}  // namespace

namespace wpi::uv {

UdpSendReq::UdpSendReq() {
  error = [this](Error err) { GetUdp().error(err); };
}

std::shared_ptr<Udp> Udp::Create(Loop& loop, unsigned int flags) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Udp>(private_init{});
  int err = uv_udp_init_ex(loop.GetRaw(), h->GetRaw(), flags);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Udp::Bind(std::string_view ip, unsigned int port, unsigned int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

void Udp::Bind6(std::string_view ip, unsigned int port, unsigned int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

void Udp::Connect(std::string_view ip, unsigned int port) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Connect(reinterpret_cast<const sockaddr&>(addr));
  }
}

void Udp::Connect6(std::string_view ip, unsigned int port) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    ReportError(err);
  } else {
    Connect(reinterpret_cast<const sockaddr&>(addr));
  }
}

sockaddr_storage Udp::GetPeer() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_udp_getpeername, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len)) {
    std::memset(&name, 0, sizeof(name));
  }
  return name;
}

sockaddr_storage Udp::GetSock() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_udp_getsockname, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len)) {
    std::memset(&name, 0, sizeof(name));
  }
  return name;
}

void Udp::SetMembership(std::string_view multicastAddr,
                        std::string_view interfaceAddr,
                        uv_membership membership) {
  SmallString<128> multicastAddrBuf{multicastAddr};
  SmallString<128> interfaceAddrBuf{interfaceAddr};
  Invoke(&uv_udp_set_membership, GetRaw(), multicastAddrBuf.c_str(),
         interfaceAddrBuf.c_str(), membership);
}

void Udp::SetSourceMembership(std::string_view multicastAddr,
                              std::string_view interfaceAddr,
                              std::string_view sourceAddr,
                              uv_membership membership) {
  SmallString<128> multicastAddrBuf{multicastAddr};
  SmallString<128> interfaceAddrBuf{interfaceAddr};
  SmallString<128> sourceAddrBuf{sourceAddr};
  Invoke(&uv_udp_set_source_membership, GetRaw(), multicastAddrBuf.c_str(),
         interfaceAddrBuf.c_str(), sourceAddrBuf.c_str(), membership);
}

void Udp::SetMulticastInterface(std::string_view interfaceAddr) {
  SmallString<128> interfaceAddrBuf{interfaceAddr};
  Invoke(&uv_udp_set_multicast_interface, GetRaw(), interfaceAddrBuf.c_str());
}

void Udp::Send(const sockaddr& addr, std::span<const Buffer> bufs,
               const std::shared_ptr<UdpSendReq>& req) {
  if (IsLoopClosing()) {
    return;
  }
  if (Invoke(&uv_udp_send, req->GetRaw(), GetRaw(), bufs.data(), bufs.size(),
             &addr, [](uv_udp_send_t* r, int status) {
               auto& h = *static_cast<UdpSendReq*>(r->data);
               if (status < 0) {
                 h.ReportError(status);
               }
               h.complete(Error(status));
               h.Release();  // this is always a one-shot
             })) {
    req->Keep();
  }
}

void Udp::Send(const sockaddr& addr, std::span<const Buffer> bufs,
               std::function<void(std::span<Buffer>, Error)> callback) {
  if (IsLoopClosing()) {
    return;
  }
  Send(addr, bufs,
       std::make_shared<CallbackUdpSendReq>(bufs, std::move(callback)));
}

void Udp::Send(std::span<const Buffer> bufs,
               const std::shared_ptr<UdpSendReq>& req) {
  if (IsLoopClosing()) {
    return;
  }
  if (Invoke(&uv_udp_send, req->GetRaw(), GetRaw(), bufs.data(), bufs.size(),
             nullptr, [](uv_udp_send_t* r, int status) {
               auto& h = *static_cast<UdpSendReq*>(r->data);
               if (status < 0) {
                 h.ReportError(status);
               }
               h.complete(Error(status));
               h.Release();  // this is always a one-shot
             })) {
    req->Keep();
  }
}

void Udp::Send(std::span<const Buffer> bufs,
               std::function<void(std::span<Buffer>, Error)> callback) {
  if (IsLoopClosing()) {
    return;
  }
  Send(bufs, std::make_shared<CallbackUdpSendReq>(bufs, std::move(callback)));
}

void Udp::StartRecv() {
  if (IsLoopClosing()) {
    return;
  }
  Invoke(&uv_udp_recv_start, GetRaw(), &AllocBuf,
         [](uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
            const sockaddr* addr, unsigned flags) {
           auto& h = *static_cast<Udp*>(handle->data);
           Buffer data = *buf;

           // nread=0 is simply ignored
           if (nread > 0) {
             h.received(data, static_cast<size_t>(nread), *addr, flags);
           } else if (nread < 0) {
             h.ReportError(nread);
           }

           // free the buffer
           h.FreeBuf(data);
         });
}

}  // namespace wpi::uv
