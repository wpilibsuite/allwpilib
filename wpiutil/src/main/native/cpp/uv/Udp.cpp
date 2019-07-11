/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Udp.h"

#include <cstring>

#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/uv/util.h"

namespace {

using namespace wpi;
using namespace wpi::uv;

class CallbackUdpSendReq : public UdpSendReq {
 public:
  CallbackUdpSendReq(
      ArrayRef<Buffer> bufs,
      std::function<void(MutableArrayRef<Buffer>, Error)> callback)
      : m_bufs{bufs.begin(), bufs.end()} {
    complete.connect([=](Error err) { callback(m_bufs, err); });
  }

 private:
  SmallVector<Buffer, 4> m_bufs;
};

}  // namespace

namespace wpi {
namespace uv {

UdpSendReq::UdpSendReq() {
  error = [this](Error err) { GetUdp().error(err); };
}

std::shared_ptr<Udp> Udp::Create(Loop& loop, unsigned int flags) {
  auto h = std::make_shared<Udp>(private_init{});
  int err = uv_udp_init_ex(loop.GetRaw(), h->GetRaw(), flags);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Udp::Bind(const Twine& ip, unsigned int port, unsigned int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
}

void Udp::Bind6(const Twine& ip, unsigned int port, unsigned int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    ReportError(err);
  else
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
}

sockaddr_storage Udp::GetSock() {
  sockaddr_storage name;
  int len = sizeof(name);
  if (!Invoke(&uv_udp_getsockname, GetRaw(), reinterpret_cast<sockaddr*>(&name),
              &len))
    std::memset(&name, 0, sizeof(name));
  return name;
}

void Udp::SetMembership(const Twine& multicastAddr, const Twine& interfaceAddr,
                        uv_membership membership) {
  SmallString<128> multicastAddrBuf;
  SmallString<128> interfaceAddrBuf;
  Invoke(&uv_udp_set_membership, GetRaw(),
         multicastAddr.toNullTerminatedStringRef(multicastAddrBuf).data(),
         interfaceAddr.toNullTerminatedStringRef(interfaceAddrBuf).data(),
         membership);
}

void Udp::SetMulticastInterface(const Twine& interfaceAddr) {
  SmallString<128> interfaceAddrBuf;
  Invoke(&uv_udp_set_multicast_interface, GetRaw(),
         interfaceAddr.toNullTerminatedStringRef(interfaceAddrBuf).data());
}

void Udp::Send(const sockaddr& addr, ArrayRef<Buffer> bufs,
               const std::shared_ptr<UdpSendReq>& req) {
  if (Invoke(&uv_udp_send, req->GetRaw(), GetRaw(), bufs.data(), bufs.size(),
             &addr, [](uv_udp_send_t* r, int status) {
               auto& h = *static_cast<UdpSendReq*>(r->data);
               if (status < 0) h.ReportError(status);
               h.complete(Error(status));
               h.Release();  // this is always a one-shot
             }))
    req->Keep();
}

void Udp::Send(const sockaddr& addr, ArrayRef<Buffer> bufs,
               std::function<void(MutableArrayRef<Buffer>, Error)> callback) {
  Send(addr, bufs, std::make_shared<CallbackUdpSendReq>(bufs, callback));
}

void Udp::StartRecv() {
  Invoke(&uv_udp_recv_start, GetRaw(), &AllocBuf,
         [](uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
            const sockaddr* addr, unsigned flags) {
           auto& h = *static_cast<Udp*>(handle->data);
           Buffer data = *buf;

           // nread=0 is simply ignored
           if (nread > 0)
             h.received(data, static_cast<size_t>(nread), *addr, flags);
           else if (nread < 0)
             h.ReportError(nread);

           // free the buffer
           h.FreeBuf(data);
         });
}

}  // namespace uv
}  // namespace wpi
