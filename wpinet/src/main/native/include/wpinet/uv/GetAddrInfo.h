// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_GETADDRINFO_H_
#define WPINET_UV_GETADDRINFO_H_

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
 * GetAddrInfo request.
 * For use with `GetAddrInfo()` function family.
 */
class GetAddrInfoReq : public RequestImpl<GetAddrInfoReq, uv_getaddrinfo_t> {
 public:
  GetAddrInfoReq();

  Loop& GetLoop() const { return *static_cast<Loop*>(GetRaw()->loop->data); }

  /**
   * Resolved lookup signal.
   * Parameter is resolved address info.
   */
  sig::Signal<const addrinfo&> resolved;
};

/**
 * Asynchronous getaddrinfo(3).  HandleResolvedAddress() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * Either node or service may be empty but not both.
 *
 * @param loop Event loop
 * @param req request
 * @param node Either a numerical network address or a network hostname.
 * @param service Either a service name or a port number as a string.
 * @param hints Optional `addrinfo` data structure with additional address
 *              type constraints.
 */
void GetAddrInfo(Loop& loop, const std::shared_ptr<GetAddrInfoReq>& req,
                 std::string_view node, std::string_view service = {},
                 const addrinfo* hints = nullptr);

/**
 * Asynchronous getaddrinfo(3).  HandleResolvedAddress() is called on the
 * request when the resolution completes.  HandleError() is called on the
 * request if any errors occur.
 *
 * Either node or service may be empty but not both.
 *
 * @param loop Event loop
 * @param req request
 * @param node Either a numerical network address or a network hostname.
 * @param service Either a service name or a port number as a string.
 * @param hints Optional `addrinfo` data structure with additional address
 *              type constraints.
 */
inline void GetAddrInfo(const std::shared_ptr<Loop>& loop,
                        const std::shared_ptr<GetAddrInfoReq>& req,
                        std::string_view node, std::string_view service = {},
                        const addrinfo* hints = nullptr) {
  GetAddrInfo(*loop, req, node, service, hints);
}

/**
 * Asynchronous getaddrinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.  This is a convenience
 * wrapper.
 *
 * Either node or service may be empty but not both.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param node Either a numerical network address or a network hostname.
 * @param service Either a service name or a port number as a string.
 * @param hints Optional `addrinfo` data structure with additional address
 *              type constraints.
 */
void GetAddrInfo(Loop& loop, std::function<void(const addrinfo&)> callback,
                 std::string_view node, std::string_view service = {},
                 const addrinfo* hints = nullptr);

/**
 * Asynchronous getaddrinfo(3).  The callback is called when the resolution
 * completes, and errors are forwarded to the loop.  This is a convenience
 * wrapper.
 *
 * Either node or service may be empty but not both.
 *
 * @param loop Event loop
 * @param callback Callback function to call when resolution completes
 * @param node Either a numerical network address or a network hostname.
 * @param service Either a service name or a port number as a string.
 * @param hints Optional `addrinfo` data structure with additional address
 *              type constraints.
 */
inline void GetAddrInfo(const std::shared_ptr<Loop>& loop,
                        std::function<void(const addrinfo&)> callback,
                        std::string_view node, std::string_view service = {},
                        const addrinfo* hints = nullptr) {
  GetAddrInfo(*loop, std::move(callback), node, service, hints);
}

}  // namespace wpi::uv

#endif  // WPINET_UV_GETADDRINFO_H_
