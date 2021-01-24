// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_HTTPWEBSOCKETSERVERCONNECTION_INL_
#define WPIUTIL_WPI_HTTPWEBSOCKETSERVERCONNECTION_INL_

#include <memory>

#include "wpi/HttpWebSocketServerConnection.h"

namespace wpi {

template <typename Derived>
HttpWebSocketServerConnection<Derived>::HttpWebSocketServerConnection(
    std::shared_ptr<uv::Stream> stream, ArrayRef<StringRef> protocols)
    : HttpServerConnection{stream},
      m_helper{m_request},
      m_protocols{protocols.begin(), protocols.end()} {
  // Handle upgrade event
  m_helper.upgrade.connect([this] {
    // Negotiate sub-protocol
    SmallVector<StringRef, 2> protocols{m_protocols.begin(), m_protocols.end()};
    StringRef protocol = m_helper.MatchProtocol(protocols).second;

    // Check that the upgrade is valid
    if (!IsValidWsUpgrade(protocol)) {
      return;
    }

    // Disconnect HttpServerConnection header reader
    m_dataConn.disconnect();
    m_messageCompleteConn.disconnect();

    // Accepting the stream may destroy this (as it replaces the stream user
    // data), so grab a shared pointer first.
    auto self = this->shared_from_this();

    // Accept the upgrade
    auto ws = m_helper.Accept(m_stream, protocol);

    // Set this as the websocket user data to keep it around
    ws->SetData(self);

    // Store in member
    m_websocket = ws.get();

    // Call derived class function
    ProcessWsUpgrade();
  });
}

}  // namespace wpi

#endif  // WPIUTIL_WPI_HTTPWEBSOCKETSERVERCONNECTION_INL_
