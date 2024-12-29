// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/DsClient.h"

#include <memory>

#include <fmt/format.h>
#include <wpi/Logger.h>
#include <wpi/StringExtras.h>
#include <wpi/json.h>

#include "wpinet/uv/Tcp.h"
#include "wpinet/uv/Timer.h"

using namespace wpi;

static constexpr uv::Timer::Time kReconnectTime{500};

DsClient::DsClient(wpi::uv::Loop& loop, wpi::Logger& logger,
                   const private_init&)
    : m_logger{logger},
      m_tcp{uv::Tcp::Create(loop)},
      m_timer{uv::Timer::Create(loop)} {
  if (!m_tcp || !m_timer) {
    return;
  }
  m_tcp->end.connect([this] {
    WPI_DEBUG4(m_logger, "DS connection closed");
    clearIp();
    // try to connect again
    m_tcp->Reuse([this] { m_timer->Start(kReconnectTime); });
  });
  m_tcp->data.connect([this](wpi::uv::Buffer buf, size_t len) {
    HandleIncoming({buf.base, len});
  });
  m_timer->timeout.connect([this] { Connect(); });
  Connect();
}

DsClient::~DsClient() = default;

void DsClient::Close() {
  m_tcp->Close();
  m_timer->Close();
  clearIp();
}

void DsClient::Connect() {
  auto connreq = std::make_shared<uv::TcpConnectReq>();
  connreq->connected.connect([this] {
    m_json.clear();
    m_tcp->StopRead();
    m_tcp->StartRead();
  });

  connreq->error = [this](uv::Error err) {
    WPI_DEBUG4(m_logger, "DS connect failure: {}", err.str());
    // try to connect again
    m_tcp->Reuse([this] { m_timer->Start(kReconnectTime); });
  };

  WPI_DEBUG4(m_logger, "Starting DS connection attempt");
  m_tcp->Connect("127.0.0.1", 1742, connreq);
}

void DsClient::HandleIncoming(std::string_view in) {
  // this is very bare-bones, as there are never nested {} in these messages
  while (!in.empty()) {
    // if json is empty, look for the first { (and discard)
    if (m_json.empty()) {
      auto start = in.find('{');
      in = wpi::slice(in, start, std::string_view::npos);
    }

    // look for the terminating } (and save)
    auto end = in.find('}');
    if (end == std::string_view::npos) {
      m_json.append(in);
      return;  // nothing left to read
    }

    // have complete json message
    ++end;
    m_json.append(wpi::slice(in, 0, end));
    in = wpi::slice(in, end, std::string_view::npos);
    ParseJson();
    m_json.clear();
  }
}

void DsClient::ParseJson() {
  WPI_DEBUG4(m_logger, "DsClient JSON: {}", m_json);
  unsigned int ip = 0;
  try {
    ip = wpi::json::parse(m_json).at("robotIP").get<unsigned int>();
  } catch (wpi::json::exception& e) {
    WPI_INFO(m_logger, "DsClient JSON error: {}", e.what());
    return;
  }

  if (ip == 0) {
    clearIp();
  } else {
    // Convert number into dotted quad
    auto newip = fmt::format("{}.{}.{}.{}", (ip >> 24) & 0xff,
                             (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    WPI_INFO(m_logger, "DS received server IP: {}", newip);
    setIp(newip);
  }
}
