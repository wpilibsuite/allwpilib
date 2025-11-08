// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "UsbCameraListener.hpp"

#include <memory>

#include <wpi/net/EventLoopRunner.hpp>
#include <wpi/net/uv/FsEvent.hpp>
#include <wpi/net/uv/Timer.hpp>

#include "Notifier.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::cs;

class UsbCameraListener::Impl {
 public:
  explicit Impl(Notifier& notifier) : m_notifier(notifier) {}

  Notifier& m_notifier;

  std::unique_ptr<wpi::net::EventLoopRunner> m_runner;
};

UsbCameraListener::UsbCameraListener(wpi::util::Logger& logger,
                                     Notifier& notifier)
    : m_impl(std::make_unique<Impl>(notifier)) {}

UsbCameraListener::~UsbCameraListener() = default;

void UsbCameraListener::Start() {
  if (!m_impl->m_runner) {
    m_impl->m_runner = std::make_unique<wpi::net::EventLoopRunner>();
    m_impl->m_runner->ExecAsync(
        [impl = m_impl.get()](wpi::net::uv::Loop& loop) {
          auto refreshTimer = wpi::net::uv::Timer::Create(loop);
          refreshTimer->timeout.connect([notifier = &impl->m_notifier] {
            notifier->NotifyUsbCamerasChanged();
          });
          refreshTimer->Unreference();

          auto devEvents = wpi::net::uv::FsEvent::Create(loop);
          devEvents->fsEvent.connect([refreshTimer](const char* fn, int flags) {
            if (wpi::util::starts_with(fn, "video")) {
              refreshTimer->Start(wpi::net::uv::Timer::Time(200));
            }
          });
          devEvents->Start("/dev");
          devEvents->Unreference();
        });
  }
}

void UsbCameraListener::Stop() {
  if (m_impl->m_runner) {
    m_impl->m_runner.reset();
  }
}
