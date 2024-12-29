// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/PortForwarder.h"

#include <memory>
#include <string>

#include <fmt/format.h>
#include <wpi/DenseMap.h>
#include <wpi/print.h>

#include "wpinet/EventLoopRunner.h"
#include "wpinet/uv/GetAddrInfo.h"
#include "wpinet/uv/Tcp.h"
#include "wpinet/uv/Timer.h"

using namespace wpi;

struct PortForwarder::Impl {
 public:
  EventLoopRunner runner;
  DenseMap<unsigned int, std::weak_ptr<uv::Tcp>> servers;
};

PortForwarder::PortForwarder() : m_impl{new Impl} {}

PortForwarder& PortForwarder::GetInstance() {
  static PortForwarder instance;
  return instance;
}

static void CopyStream(uv::Stream& in, std::weak_ptr<uv::Stream> outWeak) {
  in.data.connect([&in, outWeak](uv::Buffer& buf, size_t len) {
    uv::Buffer buf2 = buf.Dup();
    buf2.len = len;
    auto out = outWeak.lock();
    if (!out) {
      buf2.Deallocate();
      in.Close();
      return;
    }
    out->Write({buf2}, [](auto bufs, uv::Error) {
      for (auto buf : bufs) {
        buf.Deallocate();
      }
    });
  });
}

void PortForwarder::Add(unsigned int port, std::string_view remoteHost,
                        unsigned int remotePort) {
  m_impl->runner.ExecSync([&](uv::Loop& loop) {
    auto server = uv::Tcp::Create(loop);
    if (!server) {
      wpi::print(stderr, "PortForwarder: Creating server failed\n");
      return;
    }

    // bind to local port
    server->Bind("", port);

    // when we get a connection, accept it
    server->connection.connect([serverPtr = server.get(),
                                host = std::string{remoteHost}, remotePort] {
      auto& loop = serverPtr->GetLoopRef();
      auto client = serverPtr->Accept();
      if (!client) {
        wpi::print(stderr, "PortForwarder: Connecting to client failed\n");
        return;
      }

      // close on error
      client->error.connect(
          [clientPtr = client.get()](uv::Error err) { clientPtr->Close(); });

      // connected flag
      auto connected = std::make_shared<bool>(false);
      client->SetData(connected);

      auto remote = uv::Tcp::Create(loop);
      if (!remote) {
        wpi::print(stderr, "PortForwarder: Creating remote failed\n");
        client->Close();
        return;
      }
      remote->error.connect(
          [remotePtr = remote.get(),
           clientWeak = std::weak_ptr<uv::Tcp>(client)](uv::Error err) {
            remotePtr->Close();
            if (auto client = clientWeak.lock()) {
              client->Close();
            }
          });

      // resolve address
      uv::GetAddrInfo(
          loop,
          [clientWeak = std::weak_ptr<uv::Tcp>(client),
           remoteWeak = std::weak_ptr<uv::Tcp>(remote)](const addrinfo& addr) {
            auto remote = remoteWeak.lock();
            if (!remote) {
              return;
            }

            // connect to remote address/port
            remote->Connect(*addr.ai_addr, [remotePtr = remote.get(),
                                            remoteWeak, clientWeak] {
              auto client = clientWeak.lock();
              if (!client) {
                remotePtr->Close();
                return;
              }
              *(client->GetData<bool>()) = true;
              wpi::print("PortForwarder: Connected to remote port\n");

              // close both when either side closes
              client->end.connect([clientPtr = client.get(), remoteWeak] {
                clientPtr->Close();
                if (auto remote = remoteWeak.lock()) {
                  remote->Close();
                }
              });
              remotePtr->end.connect([remotePtr, clientWeak] {
                remotePtr->Close();
                if (auto client = clientWeak.lock()) {
                  client->Close();
                }
              });

              // copy bidirectionally
              client->StartRead();
              remotePtr->StartRead();
              CopyStream(*client, remoteWeak);
              CopyStream(*remotePtr, clientWeak);
            });
          },
          host, fmt::to_string(remotePort));

      // time out for connection
      uv::Timer::SingleShot(loop, uv::Timer::Time{500},
                            [connectedWeak = std::weak_ptr<bool>(connected),
                             clientWeak = std::weak_ptr<uv::Tcp>(client),
                             remoteWeak = std::weak_ptr<uv::Tcp>(remote)] {
                              if (auto connected = connectedWeak.lock()) {
                                if (!*connected) {
                                  if (auto client = clientWeak.lock()) {
                                    client->Close();
                                  }
                                  if (auto remote = remoteWeak.lock()) {
                                    remote->Close();
                                  }
                                }
                              }
                            });
    });

    // start listening for incoming connections
    server->Listen();

    m_impl->servers[port] = server;
  });
}

void PortForwarder::Remove(unsigned int port) {
  m_impl->runner.ExecSync([&](uv::Loop& loop) {
    if (auto server = m_impl->servers.lookup(port).lock()) {
      server->Close();
      m_impl->servers.erase(port);
    }
  });
}
