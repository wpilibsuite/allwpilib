// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "wpi/net/EventLoopRunner.hpp"
#include "wpi/net/uv/Udp.hpp"
#include "wpi/net/uv/util.hpp"

// A UDP peer for inspecting the actual time-sync wire traffic.
class TimeSyncTestPeer {
 public:
  struct Packet {
    std::vector<uint8_t> data;
    sockaddr_storage sender{};
  };

  TimeSyncTestPeer() {
    m_loop.ExecSync([this](auto& loop) {
      m_udp = wpi::net::uv::Udp::Create(loop, AF_INET);
      m_udp->Bind("127.0.0.1", 0);
      std::string address;
      wpi::net::uv::AddrToName(m_udp->GetSock(), &address, &m_port);
      m_udp->received.connect([this](auto& data, size_t size,
                                     const sockaddr& sender, unsigned) {
        Packet packet;
        packet.data.assign(data.bytes().begin(), data.bytes().begin() + size);
        std::memcpy(&packet.sender, &sender, sizeof(sockaddr_in));
        {
          std::scoped_lock lock{m_mutex};
          m_packets.emplace_back(std::move(packet));
        }
        m_received.notify_one();
      });
      m_udp->StartRecv();
    });
  }

  ~TimeSyncTestPeer() { m_loop.Stop(); }

  unsigned int GetPort() const { return m_port; }

  std::optional<Packet> Receive(std::chrono::milliseconds timeout) {
    std::unique_lock lock{m_mutex};
    if (!m_received.wait_for(lock, timeout,
                             [this] { return !m_packets.empty(); })) {
      return std::nullopt;
    }
    auto packet = std::move(m_packets.front());
    m_packets.pop_front();
    return packet;
  }

  int Send(std::span<const uint8_t> data, unsigned int port) {
    sockaddr_in addr;
    wpi::net::uv::NameToAddr("127.0.0.1", port, &addr);
    return Send(data, reinterpret_cast<const sockaddr&>(addr));
  }

  int Send(std::span<const uint8_t> data, const sockaddr& addr) {
    int sent = 0;
    m_loop.ExecSync([&](auto&) {
      wpi::net::uv::Buffer buffer{data};
      sent = m_udp->TrySend(addr, {&buffer, 1});
    });
    return sent;
  }

 private:
  std::mutex m_mutex;
  std::condition_variable m_received;
  std::deque<Packet> m_packets;
  unsigned int m_port = 0;
  std::shared_ptr<wpi::net::uv::Udp> m_udp;
  wpi::net::EventLoopRunner m_loop;
};
