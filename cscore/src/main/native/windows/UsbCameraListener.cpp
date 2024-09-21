// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "UsbCameraListener.h"

#include "Notifier.h"
#include "WindowsMessagePump.h"

#include <dbt.h>  // NOLINT(build/include_order)

#include <memory>

#define IDT_TIMER1 1001

using namespace cs;

class UsbCameraListener::Impl {
 public:
  explicit Impl(Notifier& notifier) : m_notifier{notifier} {}

  void Start() {
    m_messagePump = std::make_unique<WindowsMessagePump>(
        [this](HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
          return this->PumpMain(hwnd, uiMsg, wParam, lParam);
        });
  }

  void Stop() { m_messagePump = nullptr; }

  LRESULT PumpMain(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
    switch (uiMsg) {
      case WM_CLOSE:
        KillTimer(hwnd, IDT_TIMER1);
        break;
      case WM_TIMER:
        if (wParam == IDT_TIMER1) {
          KillTimer(hwnd, IDT_TIMER1);
          m_notifier.NotifyUsbCamerasChanged();
        }
        break;
      case WM_DEVICECHANGE:
        PDEV_BROADCAST_HDR parameter =
            reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
        if (wParam == DBT_DEVICEARRIVAL &&
            parameter->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
          SetTimer(hwnd, IDT_TIMER1, 200, nullptr);
        } else if (wParam == DBT_DEVICEREMOVECOMPLETE &&
                   parameter->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
          SetTimer(hwnd, IDT_TIMER1, 200, nullptr);
        }
        break;
    }
    return 0;
  }

  Notifier& m_notifier;
  std::unique_ptr<cs::WindowsMessagePump> m_messagePump;
};

UsbCameraListener::UsbCameraListener(wpi::Logger& logger, Notifier& notifier)
    : m_impl{std::make_unique<Impl>(notifier)} {}

UsbCameraListener::~UsbCameraListener() {
  Stop();
}

void UsbCameraListener::Start() {
  m_impl->Start();
}

void UsbCameraListener::Stop() {
  m_impl->Stop();
}
