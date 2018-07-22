/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_UDP_H_
#define WPIUTIL_WPI_UV_UDP_H_

#include <uv.h>

#include <functional>
#include <memory>

#include "wpi/ArrayRef.h"
#include "wpi/Signal.h"
#include "wpi/Twine.h"
#include "wpi/uv/Handle.h"
#include "wpi/uv/Request.h"

namespace wpi {
namespace uv {

class Loop;
class Udp;

/**
 * UDP send request.
 */
class UdpSendReq : public RequestImpl<UdpSendReq, uv_udp_send_t> {
 public:
  UdpSendReq();

  Udp& GetUdp() const { return *static_cast<Udp*>(GetRaw()->handle->data); }

  /**
   * Send completed signal.  This is called even if an error occurred.
   * @param err error value
   */
  sig::Signal<Error> complete;
};

/**
 * UDP handle.
 * UDP handles encapsulate UDP communication for both clients and servers.
 */
class Udp final : public HandleImpl<Udp, uv_udp_t> {
  struct private_init {};

 public:
  explicit Udp(const private_init&) {}
  ~Udp() noexcept override = default;

  /**
   * Create a UDP handle.
   *
   * @param loop Loop object where this handle runs.
   * @param flags Flags
   */
  static std::shared_ptr<Udp> Create(Loop& loop,
                                     unsigned int flags = AF_UNSPEC);

  /**
   * Create a UDP handle.
   *
   * @param loop Loop object where this handle runs.
   * @param flags Flags
   */
  static std::shared_ptr<Udp> Create(const std::shared_ptr<Loop>& loop,
                                     unsigned int flags = AF_UNSPEC) {
    return Create(*loop, flags);
  }

  /**
   * Open an existing file descriptor or SOCKET as a UDP handle.
   *
   * @param sock A valid socket handle (either a file descriptor or a SOCKET).
   */
  void Open(uv_os_sock_t sock) { Invoke(&uv_udp_open, GetRaw(), sock); }

  /**
   * Bind the handle to an IPv4 or IPv6 address and port.
   *
   * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
   * @param flags Optional additional flags.
   */
  void Bind(const sockaddr& addr, unsigned int flags = 0) {
    Invoke(&uv_udp_bind, GetRaw(), &addr, flags);
  }

  void Bind(const sockaddr_in& addr, unsigned int flags = 0) {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }

  void Bind(const sockaddr_in6& addr, unsigned int flags = 0) {
    Bind(reinterpret_cast<const sockaddr&>(addr), flags);
  }

  /**
   * Bind the handle to an IPv4 address and port.
   *
   * @param ip The address to which to bind.
   * @param port The port to which to bind.
   * @param flags Optional additional flags.
   */
  void Bind(const Twine& ip, unsigned int port, unsigned int flags = 0);

  /**
   * Bind the handle to an IPv6 address and port.
   *
   * @param ip The address to which to bind.
   * @param port The port to which to bind.
   * @param flags Optional additional flags.
   */
  void Bind6(const Twine& ip, unsigned int port, unsigned int flags = 0);

  /**
   * Get the current address to which the handle is bound.
   * @return The address (will be zeroed if an error occurred).
   */
  sockaddr_storage GetSock();

  /**
   * Set membership for a multicast address.
   *
   * @param multicastAddr Multicast address to set membership for
   * @param interfaceAddr Interface address
   * @param membership Should be UV_JOIN_GROUP or UV_LEAVE_GROUP
   */
  void SetMembership(const Twine& multicastAddr, const Twine& interfaceAddr,
                     uv_membership membership);

  /**
   * Set IP multicast loop flag.  Makes multicast packets loop back to local
   * sockets.
   *
   * @param enabled True for enabled, false for disabled
   */
  void SetMulticastLoop(bool enabled) {
    Invoke(&uv_udp_set_multicast_loop, GetRaw(), enabled ? 1 : 0);
  }

  /**
   * Set the multicast TTL.
   *
   * @param ttl Time to live (1-255)
   */
  void SetMulticastTtl(int ttl) {
    Invoke(&uv_udp_set_multicast_ttl, GetRaw(), ttl);
  }

  /**
   * Set the multicast interface to send or receive data on.
   *
   * @param interfaceAddr Interface address
   */
  void SetMulticastInterface(const Twine& interfaceAddr);

  /**
   * Set broadcast on or off.
   *
   * @param enabled True for enabled, false for disabled
   */
  void SetBroadcast(bool enabled) {
    Invoke(&uv_udp_set_broadcast, GetRaw(), enabled ? 1 : 0);
  }

  /**
   * Set the time to live (TTL).
   *
   * @param ttl Time to live (1-255)
   */
  void SetTtl(int ttl) { Invoke(&uv_udp_set_ttl, GetRaw(), ttl); }

  /**
   * Send data over the UDP socket.  If the socket has not previously been bound
   * with Bind() it will be bound to 0.0.0.0 (the "all interfaces" IPv4 address)
   * and a random port number.
   *
   * Data are written in order. The lifetime of the data pointers passed in
   * the `bufs` parameter must exceed the lifetime of the send request.
   * The callback can be used to free data after the request completes.
   *
   * HandleSendComplete() will be called on the request object when the data
   * has been written.  HandleSendComplete() is called even if an error occurs.
   * HandleError() will be called on the request object in case of errors.
   *
   * @param addr sockaddr_in or sockaddr_in6 with the address and port of the
   *             remote peer.
   * @param bufs The buffers to be written to the stream.
   * @param req write request
   */
  void Send(const sockaddr& addr, ArrayRef<Buffer> bufs,
            const std::shared_ptr<UdpSendReq>& req);

  void Send(const sockaddr_in& addr, ArrayRef<Buffer> bufs,
            const std::shared_ptr<UdpSendReq>& req) {
    Send(reinterpret_cast<const sockaddr&>(addr), bufs, req);
  }

  void Send(const sockaddr_in6& addr, ArrayRef<Buffer> bufs,
            const std::shared_ptr<UdpSendReq>& req) {
    Send(reinterpret_cast<const sockaddr&>(addr), bufs, req);
  }

  /**
   * Send data over the UDP socket.  If the socket has not previously been bound
   * with Bind() it will be bound to 0.0.0.0 (the "all interfaces" IPv4 address)
   * and a random port number.
   *
   * Data are written in order. The lifetime of the data pointers passed in
   * the `bufs` parameter must exceed the lifetime of the send request.
   * The callback can be used to free data after the request completes.
   *
   * The callback will be called when the data has been sent.  Errors will
   * be reported via the error signal.
   *
   * @param addr sockaddr_in or sockaddr_in6 with the address and port of the
   *             remote peer.
   * @param bufs The buffers to be sent.
   * @param callback Callback function to call when the data has been sent.
   */
  void Send(const sockaddr& addr, ArrayRef<Buffer> bufs,
            std::function<void(MutableArrayRef<Buffer>, Error)> callback);

  void Send(const sockaddr_in& addr, ArrayRef<Buffer> bufs,
            std::function<void(MutableArrayRef<Buffer>, Error)> callback) {
    Send(reinterpret_cast<const sockaddr&>(addr), bufs, callback);
  }

  void Send(const sockaddr_in6& addr, ArrayRef<Buffer> bufs,
            std::function<void(MutableArrayRef<Buffer>, Error)> callback) {
    Send(reinterpret_cast<const sockaddr&>(addr), bufs, callback);
  }

  /**
   * Same as Send(), but won't queue a send request if it can't be completed
   * immediately.
   *
   * @param addr sockaddr_in or sockaddr_in6 with the address and port of the
   *             remote peer.
   * @param bufs The buffers to be send.
   * @return Number of bytes sent.
   */
  int TrySend(const sockaddr& addr, ArrayRef<Buffer> bufs) {
    int val = uv_udp_try_send(GetRaw(), bufs.data(), bufs.size(), &addr);
    if (val < 0) {
      this->ReportError(val);
      return 0;
    }
    return val;
  }

  int TrySend(const sockaddr_in& addr, ArrayRef<Buffer> bufs) {
    return TrySend(reinterpret_cast<const sockaddr&>(addr), bufs);
  }

  int TrySend(const sockaddr_in6& addr, ArrayRef<Buffer> bufs) {
    return TrySend(reinterpret_cast<const sockaddr&>(addr), bufs);
  }

  /**
   * Prepare for receiving data.  If the socket has not previously been bound
   * with Bind() it is bound to 0.0.0.0 (the "all interfaces" IPv4 address) and
   * a random port number.
   *
   * A received signal will be emitted for each received data packet until
   * `StopRecv()` is called.
   */
  void StartRecv();

  /**
   * Stop listening for incoming datagrams.
   */
  void StopRecv() { Invoke(&uv_udp_recv_stop, GetRaw()); }

  /**
   * Gets the amount of queued bytes waiting to be sent.
   * @return Amount of queued bytes waiting to be sent.
   */
  size_t GetSendQueueSize() const noexcept { return GetRaw()->send_queue_size; }

  /**
   * Gets the amount of queued packets waiting to be sent.
   * @return Amount of queued packets waiting to be sent.
   */
  size_t GetSendQueueCount() const noexcept {
    return GetRaw()->send_queue_count;
  }

  /**
   * Signal generated for each incoming datagram.  Parameters are the buffer,
   * the number of bytes received, the address of the sender, and flags.
   */
  sig::Signal<Buffer&, size_t, const sockaddr&, unsigned> received;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_UDP_H_
