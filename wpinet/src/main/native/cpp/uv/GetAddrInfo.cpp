// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/GetAddrInfo.h"

#include <functional>
#include <memory>
#include <utility>

#include <wpi/SmallString.h>

#include "wpinet/uv/Loop.h"

namespace wpi::uv {

GetAddrInfoReq::GetAddrInfoReq() {
  error = [this](Error err) { GetLoop().error(err); };
}

void GetAddrInfo(Loop& loop, const std::shared_ptr<GetAddrInfoReq>& req,
                 std::string_view node, std::string_view service,
                 std::optional<addrinfo> hints) {
  if (loop.IsClosing()) {
    return;
  }
  SmallString<128> nodeStr{node};
  SmallString<128> serviceStr{service};
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
      node.empty() ? nullptr : nodeStr.c_str(),
      service.empty() ? nullptr : serviceStr.c_str(),
      hints.has_value() ? &hints.value() : nullptr);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    req->Keep();
  }
}

void GetAddrInfo(Loop& loop, std::function<void(const addrinfo&)> callback,
                 std::string_view node, std::string_view service,
                 std::optional<addrinfo> hints) {
  auto req = std::make_shared<GetAddrInfoReq>();
  req->resolved.connect(std::move(callback));
  GetAddrInfo(loop, req, node, service, hints);
}

}  // namespace wpi::uv
