// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/GetNameInfo.h"

#include <functional>
#include <memory>
#include <utility>

#include "wpinet/uv/Loop.h"
#include "wpinet/uv/util.h"

namespace wpi::uv {

GetNameInfoReq::GetNameInfoReq() {
  error = [this](Error err) { GetLoop().error(err); };
}

void GetNameInfo(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                 const sockaddr& addr, int flags) {
  if (loop.IsClosing()) {
    return;
  }
  int err = uv_getnameinfo(
      loop.GetRaw(), req->GetRaw(),
      [](uv_getnameinfo_t* req, int status, const char* hostname,
         const char* service) {
        auto& h = *static_cast<GetNameInfoReq*>(req->data);
        if (status < 0) {
          h.ReportError(status);
        } else {
          h.resolved(hostname, service);
        }
        h.Release();  // this is always a one-shot
      },
      &addr, flags);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    req->Keep();
  }
}

void GetNameInfo(Loop& loop,
                 std::function<void(const char*, const char*)> callback,
                 const sockaddr& addr, int flags) {
  auto req = std::make_shared<GetNameInfoReq>();
  req->resolved.connect(std::move(callback));
  GetNameInfo(loop, req, addr, flags);
}

void GetNameInfo4(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                  std::string_view ip, unsigned int port, int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    GetNameInfo(loop, req, reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

void GetNameInfo4(Loop& loop,
                  std::function<void(const char*, const char*)> callback,
                  std::string_view ip, unsigned int port, int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    GetNameInfo(loop, std::move(callback),
                reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

void GetNameInfo6(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                  std::string_view ip, unsigned int port, int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    GetNameInfo(loop, req, reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

void GetNameInfo6(Loop& loop,
                  std::function<void(const char*, const char*)> callback,
                  std::string_view ip, unsigned int port, int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0) {
    loop.ReportError(err);
  } else {
    GetNameInfo(loop, std::move(callback),
                reinterpret_cast<const sockaddr&>(addr), flags);
  }
}

}  // namespace wpi::uv
