/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_PARALLELREPEATTCPCONNECTOR_H_
#define WPIUTIL_WPI_UV_PARALLELREPEATTCPCONNECTOR_H_

#include <stdint.h>
#include <wpi/uv/Async.h>
#include <wpi/uv/GetAddrInfo.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Tcp.h>
#include <wpi/uv/Timer.h>
#include <wpi/uv/util.h>

#include <memory>
#include <string>
#include <utility>

namespace wpi {
namespace uv {

/**
 * Parallel Repeating TCP Connector
 * This is used to connect to a TCP server from a client, with multiple IP
 * addresses, and repeats on failure or connection closed.
 */
class ParallelRepeatTCPConnector
    : public std::enable_shared_from_this<ParallelRepeatTCPConnector> {
  struct private_init {};

 public:
  ParallelRepeatTCPConnector(Loop& loop, bool disconnectOnNewAddresses,
                             Timer::Time reconnectTime, const private_init&)
      : m_loop{loop},
        m_disconnectOnNewAddresses{disconnectOnNewAddresses},
        m_reconnectTime{reconnectTime} {}

  /**
   * Create a Parallel Repeat Connector.
   *
   * @param loop Loop object where this handle runs.
   * @param disconnectOnNewAddress true to force a disconnect on an address
   * change.
   * @param reconnectTime the time to wait before reattempting a connection.
   */
  static std::shared_ptr<ParallelRepeatTCPConnector> Create(
      Loop& loop, bool disconnectOnNewAddress, Timer::Time reconnectTime) {
    auto ptr = std::make_shared<ParallelRepeatTCPConnector>(
        loop, disconnectOnNewAddress, reconnectTime, private_init{});
    ptr->m_this = ptr;
    return ptr;
  }

  /**
   * Create a Parallel Repeat Connector.
   *
   * @param loop Loop object where this handle runs.
   * @param disconnectOnNewAddress true to force a disconnect on an address
   * change.
   * @param reconnectTime the time to wait before reattempting a connection.
   */
  static std::shared_ptr<ParallelRepeatTCPConnector> Create(
      const std::shared_ptr<Loop>& loop, bool disconnectOnNewAddress,
      Timer::Time reconnectTime) {
    return Create(*loop, disconnectOnNewAddress, reconnectTime);
  }

  /**
   * Start the repeating connection.
   *
   * This must be called before any addresses are added.
   */
  void Start();

  /**
   * Close the repeating connection.
   */
  void Close();

  /**
   * Signal generated when this connector is closed.
   */
  sig::Signal<> closed;

  /**
   * Signal generated when the client connects.
   *
   * The string parameter is the IP address of the server.
   */
  sig::Signal<Tcp&, StringRef> connected;

  /**
   * Add an address to connect to.
   *
   * This does not remove any existing addresses.
   */
  void AddAddress(const Twine& addr, uint32_t port) {
    {
      std::lock_guard<wpi::mutex> lock(m_changeAddressesMutex);
      m_changeAddresses.emplace_back(std::make_pair(addr.str(), port));
    }
    if (auto lock = m_asyncEnter.lock()) {
      lock->Send();
    }
  }

  /**
   * Add a list of addresses to connect to.
   *
   * This list will replace the existing list of addresses.
   */
  void SetAddresses(ArrayRef<std::pair<std::string, uint32_t>> addresses) {
    {
      std::lock_guard<wpi::mutex> lock(m_changeAddressesMutex);
      m_changeAddresses.clear();
      m_changeAddresses.append(addresses.begin(), addresses.end());
    }
    if (auto lock = m_asyncEnter.lock()) {
      lock->Send();
    }
  }

 private:
  void Connect();

  void Disconnect() {
    if (m_attemptingConnections.empty()) {
      Connect();
      return;
    }
    for (auto&& tcp : m_attemptingConnections) {
      tcp->Close();
    }
  }

  Loop& m_loop;
  bool m_disconnectOnNewAddresses;
  Timer::Time m_reconnectTime;

  SmallVector<Tcp*, 4> m_attemptingConnections;

  std::weak_ptr<Async<>> m_asyncEnter;
  SmallVector<std::pair<std::string, uint32_t>, 1> m_connectingAddresses;

  wpi::mutex m_changeAddressesMutex;
  SmallVector<std::pair<std::string, uint32_t>, 1> m_changeAddresses;

  std::shared_ptr<ParallelRepeatTCPConnector> m_this;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_PARALLELREPEATTCPCONNECTOR_H_
