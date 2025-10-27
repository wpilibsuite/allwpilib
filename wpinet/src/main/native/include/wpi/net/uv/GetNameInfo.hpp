// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_GETNAMEINFO_H_
#define WPINET_UV_GETNAMEINFO_H_

#include <uv.h>

#include <functional>
#include <memory>
#include <string_view>
#include <utility>

#include <wpi/Signal.h>

#include "wpinet/uv/Request.h"

namespace wpi::uv {

class Loop;

/**
 * GetNameInfo request.
 * For use with `GetNameInfo()` function family.
 */
class GetNameInfoReq : public RequestImpl<GetNameInfoReq, uv_getnameinfo_t> {
 public:
  GetNameInfoReq();

  Loop& GetLoop() const { return *static_cast<Loop*>(GetRaw()->loop->data); }

  /**
   * Resolved lookup signal.
   * Parameters are hostname and service.
   */
  sig::Signal<const char*, const char*> resolved;
};

/**
 * Asynchronous getnameinfo(3).  HandleResolvedName() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * @param loop Event loop
 * @param req request
 * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
void GetNameInfo(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                 const sockaddr& addr, int flags = 0);

/**
 * Asynchronous getnameinfo(3).  HandleResolvedName() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * @param loop Event loop
 * @param req request
 * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
inline void GetNameInfo(const std::shared_ptr<Loop>& loop,
                        const std::shared_ptr<GetNameInfoReq>& req,
                        const sockaddr& addr, int flags = 0) {
  GetNameInfo(*loop, req, addr, flags);
}

/**
 * Asynchronous getnameinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
void GetNameInfo(Loop& loop,
                 std::function<void(const char*, const char*)> callback,
                 const sockaddr& addr, int flags = 0);

/**
 * Asynchronous getnameinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
inline void GetNameInfo(const std::shared_ptr<Loop>& loop,
                        std::function<void(const char*, const char*)> callback,
                        const sockaddr& addr, int flags = 0) {
  GetNameInfo(*loop, std::move(callback), addr, flags);
}

/**
 * Asynchronous IPv4 getnameinfo(3).  HandleResolvedName() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * @param loop Event loop
 * @param req request
 * @param ip A valid IPv4 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
void GetNameInfo4(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                  std::string_view ip, unsigned int port, int flags = 0);

/**
 * Asynchronous IPv4 getnameinfo(3).  HandleResolvedName() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * @param loop Event loop
 * @param req request
 * @param ip A valid IPv4 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
inline void GetNameInfo4(const std::shared_ptr<Loop>& loop,
                         const std::shared_ptr<GetNameInfoReq>& req,
                         std::string_view ip, unsigned int port,
                         int flags = 0) {
  return GetNameInfo4(*loop, req, ip, port, flags);
}

/**
 * Asynchronous IPv4 getnameinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param ip A valid IPv4 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
void GetNameInfo4(Loop& loop,
                  std::function<void(const char*, const char*)> callback,
                  std::string_view ip, unsigned int port, int flags = 0);

/**
 * Asynchronous IPv4 getnameinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.  This is a convenience
 * wrapper.
 *
 * @param loop Event loop
 * @param ip A valid IPv4 address
 * @param port A valid port number
 * @param callback Callback function to call when resolution completes
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
inline void GetNameInfo4(const std::shared_ptr<Loop>& loop,
                         std::function<void(const char*, const char*)> callback,
                         std::string_view ip, unsigned int port,
                         int flags = 0) {
  return GetNameInfo4(*loop, std::move(callback), ip, port, flags);
}

/**
 * Asynchronous IPv6 getnameinfo(3).  HandleResolvedName() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * @param loop Event loop
 * @param req request
 * @param ip A valid IPv6 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
void GetNameInfo6(Loop& loop, const std::shared_ptr<GetNameInfoReq>& req,
                  std::string_view ip, unsigned int port, int flags = 0);

/**
 * Asynchronous IPv6 getnameinfo(3).  HandleResolvedName() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * @param loop Event loop
 * @param req request
 * @param ip A valid IPv6 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
inline void GetNameInfo6(const std::shared_ptr<Loop>& loop,
                         const std::shared_ptr<GetNameInfoReq>& req,
                         std::string_view ip, unsigned int port,
                         int flags = 0) {
  GetNameInfo6(*loop, req, ip, port, flags);
}

/**
 * Asynchronous IPv6 getnameinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.  This is a convenience
 * wrapper.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param ip A valid IPv6 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
void GetNameInfo6(Loop& loop,
                  std::function<void(const char*, const char*)> callback,
                  std::string_view ip, unsigned int port, int flags = 0);

/**
 * Asynchronous IPv6 getnameinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.  This is a convenience
 * wrapper.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param ip A valid IPv6 address
 * @param port A valid port number
 * @param flags Optional flags to modify the behavior of `getnameinfo`.
 */
inline void GetNameInfo6(const std::shared_ptr<Loop>& loop,
                         std::function<void(const char*, const char*)> callback,
                         std::string_view ip, unsigned int port,
                         int flags = 0) {
  return GetNameInfo6(*loop, std::move(callback), ip, port, flags);
}

}  // namespace wpi::uv

#endif  // WPINET_UV_GETNAMEINFO_H_
