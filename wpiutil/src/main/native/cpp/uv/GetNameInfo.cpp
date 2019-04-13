/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/GetNameInfo.h"

#include "wpi/uv/Loop.h"
#include "wpi/uv/util.h"

namespace wpi {
namespace uv {

GetNameInfoReq::GetNameInfoReq() {
  error = [this](Error err) { GetLoop().error(err); };
}

void GetNameInfo(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                 const sockaddr& addr, int flags) {
  int err = uv_getnameinfo(loop.GetRaw(), req->GetRaw(),
                           [](uv_getnameinfo_t* req, int status,
                              const char* hostname, const char* service) {
                             auto& h = *static_cast<GetNameInfoReq*>(req->data);
                             if (status < 0)
                               h.ReportError(status);
                             else
                               h.resolved(hostname, service);
                             h.Release();  // this is always a one-shot
                           },
                           &addr, flags);
  if (err < 0)
    loop.ReportError(err);
  else
    req->Keep();
}

void GetNameInfo(Loop& loop,
                 std::function<void(const char*, const char*)> callback,
                 const sockaddr& addr, int flags) {
  auto req = std::make_shared<GetNameInfoReq>();
  req->resolved.connect(callback);
  GetNameInfo(loop, req, addr, flags);
}

void GetNameInfo4(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                  const Twine& ip, unsigned int port, int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    loop.ReportError(err);
  else
    GetNameInfo(loop, req, reinterpret_cast<const sockaddr&>(addr), flags);
}

void GetNameInfo4(Loop& loop,
                  std::function<void(const char*, const char*)> callback,
                  const Twine& ip, unsigned int port, int flags) {
  sockaddr_in addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    loop.ReportError(err);
  else
    GetNameInfo(loop, callback, reinterpret_cast<const sockaddr&>(addr), flags);
}

void GetNameInfo6(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                  const Twine& ip, unsigned int port, int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    loop.ReportError(err);
  else
    GetNameInfo(loop, req, reinterpret_cast<const sockaddr&>(addr), flags);
}

void GetNameInfo6(Loop& loop,
                  std::function<void(const char*, const char*)> callback,
                  const Twine& ip, unsigned int port, int flags) {
  sockaddr_in6 addr;
  int err = NameToAddr(ip, port, &addr);
  if (err < 0)
    loop.ReportError(err);
  else
    GetNameInfo(loop, callback, reinterpret_cast<const sockaddr&>(addr), flags);
}

}  // namespace uv
}  // namespace wpi
