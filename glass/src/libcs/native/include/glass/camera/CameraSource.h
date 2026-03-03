// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <cscore_cpp.h>
#include <cscore_cv.h>
#include <imgui.h>
#include <wpi/Signal.h>
#include <wpi/StringMap.h>
#include <wpi/spinlock.h>
#include <wpigui.h>

namespace glass {

/**
 * A data source for camera data.
 */
class CameraSource {
 public:
  explicit CameraSource(std::string_view id, cs::VideoSource source)
      : m_id{id}, m_source{source} {}
  virtual ~CameraSource();

  CameraSource(const CameraSource&) = delete;
  CameraSource& operator=(const CameraSource&) = delete;

  const char* GetId() const { return m_id.c_str(); }

  void UpdateTexture();

  wpi::gui::Texture& GetTexture() { return m_tex; }

  void Start();
  void Stop();
  bool IsRunning() const { return m_frameThread.joinable(); }

  /**
   * Get the source handle.
   */
  cs::VideoSource& GetSource() { return m_source; }

  /**
   * Get the actual FPS.
   *
   * @return Actual FPS averaged over a 1 second period.
   */
  double GetActualFPS() { return m_source.GetActualFPS(); }

  /**
   * Get the data rate (in bytes per second).
   *
   * <p>SetTelemetryPeriod() must be called for this to be valid.
   *
   * @return Data rate averaged over a 1 second period.
   */
  double GetActualDataRate() { return m_source.GetActualDataRate(); }

  // drag source helpers
  void LabelText(const char* label, const char* fmt, ...) const IM_FMTARGS(3);
  void LabelTextV(const char* label, const char* fmt, va_list args) const
      IM_FMTLIST(3);
  void EmitDrag(ImGuiDragDropFlags flags = 0) const;

  static CameraSource* Find(std::string_view id);

  static wpi::sig::Signal<const char*, CameraSource*> sourceCreated;

 private:
  cv::Mat* AllocMat();

  static wpi::StringMap<CameraSource*> sources;

  std::string m_id;
  cs::VideoSource m_source;

  std::atomic<cv::Mat*> m_latestFrame{nullptr};
  std::vector<cv::Mat*> m_sharedFreeList;
  wpi::spinlock m_sharedFreeListMutex;
  std::vector<cv::Mat*> m_sourceFreeList;
  std::atomic<bool> m_stopCamera{false};
  std::thread m_frameThread;

  wpi::gui::Texture m_tex;
};

}  // namespace glass
