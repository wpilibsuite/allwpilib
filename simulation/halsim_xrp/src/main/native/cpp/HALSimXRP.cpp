// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimXRP.h"

#include <cstdio>
#include <string>

#include <wpi/Endian.h>
#include <wpi/MathExtras.h>
#include <wpi/SmallString.h>
#include <wpi/print.h>
#include <wpinet/raw_uv_ostream.h>
#include <wpinet/uv/util.h>

namespace uv = wpi::uv;

using namespace wpilibxrp;

HALSimXRP::HALSimXRP(wpi::uv::Loop& loop,
                     wpilibws::ProviderContainer& providers,
                     wpilibws::HALSimWSProviderSimDevices& simDevicesProvider)
    : m_loop(loop),
      m_providers(providers),
      m_simDevicesProvider(simDevicesProvider) {
  m_loop.error.connect([](uv::Error err) {
    wpi::print(stderr, "HALSim XRP Client libuv Error: {}\n", err.str());
  });

  m_udp_client = uv::Udp::Create(m_loop);
  m_exec = UvExecFunc::Create(m_loop);
  if (m_exec) {
    m_exec->wakeup.connect([](auto func) { func(); });
  }
}

bool HALSimXRP::Initialize() {
  if (!m_udp_client || !m_exec) {
    return false;
  }

  const char* host = std::getenv("HALSIMXRP_HOST");
  if (host != nullptr) {
    m_host = host;
  } else {
    m_host = "localhost";
  }

  const char* port = std::getenv("HALSIMXRP_PORT");
  if (port != nullptr) {
    try {
      m_port = std::stoi(port);
    } catch (const std::invalid_argument& err) {
      wpi::print(stderr, "Error decoding HALSIMXRP_PORT ({})\n", err.what());
      return false;
    }
  } else {
    m_port = 3540;
  }

  wpilibxrp::WPILibUpdateFunc func = [&](const wpi::json& data) {
    OnNetValueChanged(data);
  };

  m_xrp.SetWPILibUpdateFunc(func);

  return true;
}

void HALSimXRP::Start() {
  // struct sockaddr_in dest;
  uv::NameToAddr(m_host, m_port, &m_dest);

  m_udp_client->Connect(m_dest);

  m_udp_client->received.connect(
      [this, socket = m_udp_client.get()](auto data, size_t len, auto rinfo,
                                          unsigned int flags) {
        ParsePacket({reinterpret_cast<uint8_t*>(data.base), len});
      });

  m_udp_client->closed.connect([]() { wpi::print("Socket Closed\n"); });

  // Fake the OnNetworkConnected call
  auto hws = shared_from_this();
  m_simDevicesProvider.OnNetworkConnected(hws);
  m_providers.ForEach(
      [hws](std::shared_ptr<wpilibws::HALSimWSBaseProvider> provider) {
        provider->OnNetworkConnected(hws);
      });

  m_udp_client->StartRecv();

  std::puts("HALSimXRP Initialized");
}

void HALSimXRP::ParsePacket(std::span<const uint8_t> packet) {
  if (packet.size() < 3) {
    return;
  }

  // Hand this off to the XRP object to deal with the messages
  m_xrp.HandleXRPUpdate(packet);
}

void HALSimXRP::OnNetValueChanged(const wpi::json& msg) {
  try {
    auto& type = msg.at("type").get_ref<const std::string&>();
    auto& device = msg.at("device").get_ref<const std::string&>();

    wpi::SmallString<64> key;
    key.append(type);
    if (!device.empty()) {
      key.append("/");
      key.append(device);
    }

    auto provider = m_providers.Get(key.str());
    if (provider) {
      provider->OnNetValueChanged(msg.at("data"));
    }
  } catch (wpi::json::exception& e) {
    wpi::print(stderr, "Error with incoming message: {}\n", e.what());
  }
}

void HALSimXRP::OnSimValueChanged(const wpi::json& simData) {
  // We'll use a signal from robot code to send all the data
  if (simData["type"] == "HAL") {
    auto halData = simData["data"];
    if (halData.find(">sim_periodic_after") != halData.end()) {
      SendStateToXRP();
    }
  } else {
    m_xrp.HandleWPILibUpdate(simData);
  }
}

uv::SimpleBufferPool<4>& HALSimXRP::GetBufferPool() {
  static uv::SimpleBufferPool<4> bufferPool(128);
  return bufferPool;
}

void HALSimXRP::SendStateToXRP() {
  wpi::SmallVector<uv::Buffer, 4> sendBufs;
  wpi::raw_uv_ostream stream{sendBufs, [&] {
                               std::lock_guard lock(m_buffer_mutex);
                               return GetBufferPool().Allocate();
                             }};
  m_xrp.SetupXRPSendBuffer(stream);

  m_exec->Send([this, sendBufs]() mutable {
    m_udp_client->Send(sendBufs, [&](auto bufs, uv::Error err) {
      {
        std::lock_guard lock(m_buffer_mutex);
        GetBufferPool().Release(bufs);
      }

      if (err) {
        // no-op
      }
    });
  });
}
