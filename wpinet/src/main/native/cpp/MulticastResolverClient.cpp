// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/MulticastResolverClient.hpp"

#include <memory>
#include <string>
#include <utility>

#include "wpi/net/uv/Async.hpp"
#include "wpi/net/uv/Loop.hpp"
#include "wpi/net/uv/Timer.hpp"
#include "wpi/util/Logger.hpp"

using namespace wpi::net;

static constexpr uv::Timer::Time kReconnectTime{1000};

std::shared_ptr<MulticastResolverClient> MulticastResolverClient::Create(
    wpi::net::uv::Loop& loop, wpi::util::Logger& logger,
    std::string_view serviceType) {
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto client = std::make_shared<MulticastResolverClient>(
      loop, logger, serviceType, private_init{});
  client->Init();
  return client;
}

MulticastResolverClient::MulticastResolverClient(
    wpi::net::uv::Loop& loop, wpi::util::Logger& logger,
    std::string_view serviceType, const private_init&)
    : m_loop{loop}, m_logger{logger}, m_serviceType{serviceType} {}

MulticastResolverClient::~MulticastResolverClient() = default;

void MulticastResolverClient::Init() {
  m_resolverData = uv::Async<ResolverData>::Create(m_loop);
  m_timer = uv::Timer::Create(m_loop);
  if (!m_resolverData || !m_timer) {
    return;
  }

  auto selfWeak = weak_from_this();
  m_resolverData->wakeup.connect([selfWeak](ResolverData data) {
    if (auto self = selfWeak.lock()) {
      self->HandleResolved(std::move(data));
    }
  });
  m_timer->timeout.connect([selfWeak] {
    if (auto self = selfWeak.lock()) {
      self->Restart();
    }
  });

  Restart();
  m_timer->Start(kReconnectTime, kReconnectTime);
}

void MulticastResolverClient::Close() {
  Stop();
  if (m_timer) {
    m_timer->Close();
    m_timer.reset();
  }
  if (m_resolverData) {
    m_resolverData->Close();
    m_resolverData.reset();
  }
}

void MulticastResolverClient::Restart() {
  Stop();

  auto resolver = std::make_unique<MulticastServiceResolver>(m_serviceType);
  if (!resolver->HasImplementation()) {
    if (!m_warnedNoImplementation) {
      WPI_WARNING(m_logger,
                  "mDNS service resolver not available; cannot resolve '{}'",
                  m_serviceType);
      m_warnedNoImplementation = true;
    }
    return;
  }

  std::weak_ptr<uv::Async<ResolverData>> asyncWeak = m_resolverData;
  if (!resolver->SetMoveCallback(
          [asyncWeak, generation = m_generation](
              MulticastServiceResolver::ServiceData&& data) {
            if (auto async = asyncWeak.lock(); async && !async->IsClosing()) {
              async->Send(ResolverData{generation, std::move(data)});
            }
          })) {
    return;
  }

  resolver->Start();
  m_resolver = std::move(resolver);
}

void MulticastResolverClient::Stop() {
  ++m_generation;
  m_resolver.reset();
}

void MulticastResolverClient::HandleResolved(ResolverData data) {
  if (data.generation != m_generation) {
    return;
  }
  serviceResolved(std::move(data.data));
}
