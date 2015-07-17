/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Dispatcher.h"

#include "tcpsockets/TCPAcceptor.h"
#include "tcpsockets/TCPConnector.h"

using namespace nt;

std::unique_ptr<Dispatcher> Dispatcher::m_instance;

static NT_Type GetEntryType(unsigned int id) {
  // TODO
  return NT_UNASSIGNED;
}

Dispatcher::Dispatcher() : m_active(false) {}

Dispatcher::~Dispatcher() { Stop(); }

void Dispatcher::StartServer(const char* listen_address, unsigned int port) {
  if (m_active) return;
  m_active = true;
  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread =
      std::thread(&Dispatcher::ServerThreadMain, this, listen_address, port);
}

void Dispatcher::StartClient(const char* server_name, unsigned int port) {
  if (m_active) return;
  m_active = true;
  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread =
      std::thread(&Dispatcher::ClientThreadMain, this, server_name, port);
}

void Dispatcher::Stop() {
  m_active = false;
  if (m_dispatch_thread.joinable()) m_dispatch_thread.join();
  if (m_clientserver_thread.joinable()) m_clientserver_thread.join();
}

void Dispatcher::SetUpdateRate(double interval) {
  // TODO
}

void Dispatcher::SetIdentity(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_identity = name;
}

void Dispatcher::DispatchThreadMain() {
  // TODO
}

void Dispatcher::ServerThreadMain(const char* listen_address,
                                  unsigned int port) {
  TCPAcceptor acceptor(static_cast<int>(port), listen_address);
  if (acceptor.start() != 0) {
    m_active = false;
    return;
  }
  while (m_active) {
    auto stream = acceptor.accept();
    if (!stream) {
      m_active = false;
      break;
    }
    // add to connections list
  }
}

void Dispatcher::ClientThreadMain(const char* server_name, unsigned int port) {
  std::string self_id;
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    self_id = m_identity;
  }
  unsigned int proto_rev = 0x0300;
  std::unique_ptr<NetworkConnection> conn;
  while (m_active) {
    // try to connect (with timeout)
    auto stream = TCPConnector::connect(server_name, static_cast<int>(port), 1);
    if (!stream) continue;  // keep retrying

    conn.reset(new NetworkConnection(std::move(stream), GetEntryType));
    conn->set_proto_rev(proto_rev);

    // send client hello
    conn->outgoing().push(
        NetworkConnection::Outgoing{Message::ClientHello(self_id)});

    // wait for response
    auto resp = conn->incoming().pop();

    if (resp->Is(Message::kProtoUnsup)) {
      // reconnect with lower protocol (if possible)
      if (proto_rev <= 0x0200) {
        // no more options, abort (but keep trying to connect)
        proto_rev = 0x0300;
        continue;
      }
      proto_rev = 0x0200;
      continue;
    }

    if (!resp->Is(Message::kServerHello)) {
      // shouldn't happen; disconnect but keep trying to connect
      proto_rev = 0x0300;
      continue;
    }

    // add to connections list (the dispatcher thread will handle from here)

    // block until told to reconnect
  }
}
