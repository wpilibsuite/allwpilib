// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/SystemCoreResolverClient.hpp"

#include <stdint.h>

#include <format>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "wpi/net/uv/Loop.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::net;

static constexpr std::string_view kSystemCoreServiceType = "_SystemCore._tcp";
static constexpr std::string_view kSystemCoreServicePrefix = "SystemCore-FIRST";

static std::string Ipv4AddressToString(uint32_t address) {
  return std::format("{}.{}.{}.{}", (address >> 24) & 0xff,
                     (address >> 16) & 0xff, (address >> 8) & 0xff,
                     address & 0xff);
}

static bool HasMatchingTeam(const MulticastResolverClient::ServiceData& data,
                            std::string_view team) {
  for (auto&& [key, value] : data.txt) {
    if (key == "team") {
      return wpi::util::trim(value) == team;
    }
  }
  return false;
}

std::shared_ptr<SystemCoreResolverClient> SystemCoreResolverClient::Create(
    wpi::net::uv::Loop& loop, wpi::util::Logger& logger, unsigned int port) {
  auto client = std::make_shared<SystemCoreResolverClient>(
      loop, logger, std::nullopt, port, private_init{});
  if (!client->Init()) {
    return nullptr;
  }
  return client;
}

std::shared_ptr<SystemCoreResolverClient> SystemCoreResolverClient::Create(
    wpi::net::uv::Loop& loop, wpi::util::Logger& logger, std::string_view team,
    unsigned int port) {
  auto client = std::make_shared<SystemCoreResolverClient>(
      loop, logger, std::string{wpi::util::trim(team)}, port, private_init{});
  if (!client->Init()) {
    return nullptr;
  }
  return client;
}

SystemCoreResolverClient::SystemCoreResolverClient(
    wpi::net::uv::Loop& loop, wpi::util::Logger& logger,
    std::optional<std::string> team, unsigned int port, const private_init&)
    : m_loop{loop}, m_logger{logger}, m_team{std::move(team)}, m_port{port} {}

SystemCoreResolverClient::~SystemCoreResolverClient() = default;

bool SystemCoreResolverClient::Init() {
  m_resolver =
      MulticastResolverClient::Create(m_loop, m_logger, kSystemCoreServiceType);
  if (!m_resolver) {
    return false;
  }

  auto selfWeak = weak_from_this();
  m_resolver->serviceResolved.connect(
      [selfWeak](MulticastResolverClient::ServiceData data) {
        if (auto self = selfWeak.lock()) {
          self->HandleResolved(std::move(data));
        }
      });
  return true;
}

void SystemCoreResolverClient::Close() {
  if (m_resolver) {
    m_resolver->Close();
    m_resolver.reset();
  }
}

void SystemCoreResolverClient::HandleResolved(
    MulticastResolverClient::ServiceData data) {
  if (!wpi::util::starts_with(data.serviceName, kSystemCoreServicePrefix)) {
    return;
  }

  if (m_team && !HasMatchingTeam(data, *m_team)) {
    return;
  }

  serverResolved(ServerData{std::move(data.serviceName),
                            Ipv4AddressToString(data.ipv4Address), m_port});
}
