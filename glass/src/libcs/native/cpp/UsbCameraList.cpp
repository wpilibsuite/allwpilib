// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/UsbCameraList.h"

#include <cscore_cpp.h>
#include <fmt/format.h>
#include <imgui.h>

using namespace glass;

struct UsbCameraList::UsbInfoThread : public wpi::SafeThread {
  void Main() override;

  std::vector<cs::UsbCameraInfo> m_usbInfo;
};

void UsbCameraList::UsbInfoThread::Main() {
  std::unique_lock lock{m_mutex};
  while (m_active) {
    // enumerate without lock held
    lock.unlock();
    CS_Status status = 0;
    auto usbInfo = cs::EnumerateUsbCameras(&status);

    lock.lock();
    m_usbInfo = std::move(usbInfo);

    // wait for change
    m_cond.wait(lock);
  }
}

UsbCameraList::UsbCameraList() : m_poller{cs::CreateListenerPoller()} {
  CS_Status status = 0;
  cs::AddPolledListener(m_poller, CS_USB_CAMERAS_CHANGED, false, &status);

  m_usbInfoThread.Start();
}

UsbCameraList::~UsbCameraList() {
  cs::DestroyListenerPoller(m_poller);
}

std::string UsbCameraList::DisplayMenu() {
  std::string rv;
  bool any = false;
  if (auto thr = m_usbInfoThread.GetThread()) {
    // TODO: hide cameras that already exist as sources
    for (auto&& info : thr->m_usbInfo) {
      ImGui::SeparatorText(fmt::format("{} ({:04x}:{:04x})", info.name,
                                       info.vendorId, info.productId)
                               .c_str());
      if (ImGui::MenuItem(info.path.c_str())) {
        rv = info.path;
      }
      for (auto&& path : info.otherPaths) {
        if (ImGui::MenuItem(path.c_str())) {
          rv = info.path;
        }
      }
    }
    any = !thr->m_usbInfo.empty();
  }
  if (!any) {
    ImGui::MenuItem("(None)", nullptr, false, false);
  }
  return rv;
}

void UsbCameraList::Update() {
  bool timedOut = false;
  for (auto&& event : cs::PollListener(m_poller, 0, &timedOut)) {
    if (event.kind == cs::RawEvent::kUsbCamerasChanged) {
      // enumerate on separate thread
      if (auto thr = m_usbInfoThread.GetThread()) {
        thr->m_cond.notify_all();
      }
    }
  }
}
