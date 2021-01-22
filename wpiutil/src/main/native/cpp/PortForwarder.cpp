// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/PortForwarder.h"

#include "wpi/DenseMap.h"
#include "wpi/EventLoopRunner.h"
#include "wpi/SmallString.h"
#include "wpi/raw_ostream.h"
#include "wpi/uv/GetAddrInfo.h"
#include "wpi/uv/Tcp.h"
#include "wpi/uv/Timer.h"

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
    out->Write(buf2, [](auto bufs, uv::Error) {
      for (auto buf : bufs) {
        buf.Deallocate();
      }
    });
  });
}

void PortForwarder::Add(unsigned int port, const Twine& remoteHost,
                        unsigned int remotePort) {
  m_impl->runner.ExecSync([&](uv::Loop& loop) {
    auto server = uv::Tcp::Create(loop);

    // bind to local port
    server->Bind("", port);

    // when we get a connection, accept it
    server->connection.connect([serverPtr = server.get(),
                                host = remoteHost.str(), remotePort] {
      auto& loop = serverPtr->GetLoopRef();
      auto client = serverPtr->Accept();
      if (!client) {
        return;
      }

      // close on error
      client->error.connect(
          [clientPtr = client.get()](uv::Error err) { clientPtr->Close(); });

      // connected flag
      auto connected = std::make_shared<bool>(false);
      client->SetData(connected);

      auto remote = uv::Tcp::Create(loop);
      remote->error.connect(
          [remotePtr = remote.get(),
           clientWeak = std::weak_ptr<uv::Tcp>(client)](uv::Error err) {
            remotePtr->Close();
            if (auto client = clientWeak.lock()) {
              client->Close();
            }
          });

      // convert port to string
      SmallString<16> remotePortStr;
      raw_svector_ostream(remotePortStr) << remotePort;

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
          host, remotePortStr);

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
