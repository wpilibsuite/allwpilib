// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/UsbCameraProvider.h"

#include <algorithm>

#include <cscore_cpp.h>
#include <fmt/format.h>

#include "glass/camera/Camera.h"

using namespace glass;

struct UsbCameraProvider::UsbInfoThread : public wpi::SafeThread {
  void Main() override;

  struct UsbSourceInfo : public SourceInfo {
    cs::UsbCameraInfo usb;
  };

  std::vector<UsbSourceInfo> m_sourceInfo;
};

void UsbCameraProvider::UsbInfoThread::Main() {
  std::unique_lock lock{m_mutex};
  while (m_active) {
    // enumerate without lock held
    lock.unlock();
    CS_Status status = 0;
    auto usbInfo = cs::EnumerateUsbCameras(&status);

    lock.lock();

    // set everything to not connected
    for (auto&& i : m_sourceInfo) {
      i.connected = false;
    }

    // for each USB camera, set connected or append if not found in list
    // this is not very algorithmically efficient, but N is always small
    bool resort = false;
    for (auto&& i : usbInfo) {
      auto it =
          std::find_if(m_sourceInfo.begin(), m_sourceInfo.end(),
                       [&](const auto& info) { return info.usb.dev == i.dev; });
      if (it == m_sourceInfo.end()) {
        m_sourceInfo.emplace_back();
        m_sourceInfo.back().name = i.name;
        m_sourceInfo.back().connected = true;
        m_sourceInfo.back().usb = std::move(i);
        resort = true;
      } else {
        it->connected = true;
        it->usb = std::move(i);
      }
    }
    if (resort) {
      std::sort(
          m_sourceInfo.begin(), m_sourceInfo.end(),
          [](const auto& a, const auto& b) { return a.usb.dev < b.usb.dev; });
    }

    // wait for change
    m_cond.wait(lock);
  }
}

UsbCameraProvider::UsbCameraProvider(Storage& storage)
    : CameraProviderBase{storage}, m_poller{cs::CreateListenerPoller()} {
  CS_Status status = 0;
  cs::AddPolledListener(m_poller, CS_USB_CAMERAS_CHANGED, false, &status);

  m_usbInfoThread.Start();
}

UsbCameraProvider::~UsbCameraProvider() {
  cs::DestroyListenerPoller(m_poller);
}

void UsbCameraProvider::DisplayMenu() {
  bool any = false;
  if (auto thr = m_usbInfoThread.GetThread()) {
    for (auto&& info : thr->m_sourceInfo) {
      if (info.connected) {
        MenuItem(info.name.c_str(), &info);
        any = true;
      }
    }
  }
  if (!any) {
    ImGui::MenuItem("(None)", nullptr, false, false);
  }
}

void UsbCameraProvider::Update() {
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

void UsbCameraProvider::InitCamera(SourceInfo* info) {
  info->camera = CreateModel(info->name);
  CS_Status status = 0;
  auto source = cs::CreateUsbCameraPath(
      fmt::format("glass::usb::{}", info->name),
      static_cast<UsbInfoThread::UsbSourceInfo*>(info)->usb.path, &status);
  info->camera->SetSource(source);
}
