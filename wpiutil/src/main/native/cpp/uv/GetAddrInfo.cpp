// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/uv/GetAddrInfo.h"

#include "wpi/uv/Loop.h"
#include "wpi/uv/util.h"

namespace wpi::uv {

GetAddrInfoReq::GetAddrInfoReq() {
  error = [this](Error err) { GetLoop().error(err); };
}

void GetAddrInfo(Loop& loop, const std::shared_ptr<GetAddrInfoReq>& req,
                 const Twine& node, const Twine& service,
                 const addrinfo* hints) {
  SmallVector<char, 128> nodeStr;
  SmallVector<char, 128> serviceStr;
  int err = uv_getaddrinfo(
      loop.GetRaw(), req->GetRaw(),
      [](uv_getaddrinfo_t* req, int status, addrinfo* res) {
        auto& h = *static_cast<GetAddrInfoReq*>(req->data);
        if (status < 0) {
          h.ReportError(status);
        } else {
          h.resolved(*res);
        }
        uv_freeaddrinfo(res);
        h.Release();  // this is always a one-shot
      },
      node.isNull() ? nullptr : node.toNullTerminatedStringRef(nodeStr).data(),
      service.isNull() ? nullptr
                       : service.toNullTerminatedStringRef(serviceStr).data(),
      hints);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    req->Keep();
  }
}

void GetAddrInfo(Loop& loop, std::function<void(const addrinfo&)> callback,
                 const Twine& node, const Twine& service,
                 const addrinfo* hints) {
  auto req = std::make_shared<GetAddrInfoReq>();
  req->resolved.connect(callback);
  GetAddrInfo(loop, req, node, service, hints);
}

}  // namespace wpi::uv
