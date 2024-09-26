// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "UsbCameraListener.h"

#include <memory>

#include <wpi/StringExtras.h>
#include <wpinet/EventLoopRunner.h>
#include <wpinet/uv/FsEvent.h>
#include <wpinet/uv/Timer.h>

#include "Notifier.h"

using namespace cs;

class UsbCameraListener::Impl {
 public:
  explicit Impl(Notifier& notifier) : m_notifier(notifier) {}

  Notifier& m_notifier;

  std::unique_ptr<wpi::EventLoopRunner> m_runner;
};

UsbCameraListener::UsbCameraListener(wpi::Logger& logger, Notifier& notifier)
    : m_impl(std::make_unique<Impl>(notifier)) {}

UsbCameraListener::~UsbCameraListener() = default;

void UsbCameraListener::Start() {
  if (!m_impl->m_runner) {
    m_impl->m_runner = std::make_unique<wpi::EventLoopRunner>();
    m_impl->m_runner->ExecAsync([impl = m_impl.get()](wpi::uv::Loop& loop) {
      auto refreshTimer = wpi::uv::Timer::Create(loop);
      refreshTimer->timeout.connect([notifier = &impl->m_notifier] {
        notifier->NotifyUsbCamerasChanged();
      });
      refreshTimer->Unreference();

      auto devEvents = wpi::uv::FsEvent::Create(loop);
      devEvents->fsEvent.connect([refreshTimer](const char* fn, int flags) {
        if (wpi::starts_with(fn, "video")) {
          refreshTimer->Start(wpi::uv::Timer::Time(200));
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
