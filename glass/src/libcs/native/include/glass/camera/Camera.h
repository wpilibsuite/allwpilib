// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <cscore_cpp.h>
#include <cscore_cv.h>
#include <imgui.h>
#include <wpi/ArrayRef.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>
#include <wpi/spinlock.h>
#include <wpigui.h>

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class CameraModel : public Model {
 public:
  explicit CameraModel(const wpi::Twine& name);
  ~CameraModel() override;

  CameraModel(const CameraModel&) = delete;
  CameraModel& operator=(const CameraModel&) = delete;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

  void Start();
  void Stop();

  const std::string& GetName() { return m_name; }

  wpi::gui::Texture& GetTexture() { return m_tex; }

  /**
   * Set the source handle.
   */
  void SetSource(CS_Source source);

  /**
   * Get the source handle.
   */
  CS_Source GetSource() { return m_source; }

  /**
   * Enable or disable the camera feed.
   *
   * @param enabled True to enable, false to disable
   */
  void SetEnabled(bool enabled);

  /**
   * Get the actual FPS.
   *
   * @return Actual FPS averaged over a 1 second period.
   */
  double GetActualFPS();

  /**
   * Get the data rate (in bytes per second).
   *
   * <p>SetTelemetryPeriod() must be called for this to be valid.
   *
   * @return Data rate averaged over a 1 second period.
   */
  double GetActualDataRate();

  /**
   * For a HTTP camera, change the URLs used to connect to the camera.
   */
  void SetUrls(wpi::ArrayRef<std::string> urls);

  CS_SourceKind GetKind() const;

  cs::VideoMode GetVideoMode() const;
  void SetVideoMode(const cs::VideoMode& mode);
  void ResetVideoMode();

  void ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out_buf);

 private:
  cv::Mat* AllocMat();

  std::string m_name;
  CS_Source m_source{0};
  CS_Sink m_cvSink{0};

  cs::VideoMode m_videoMode;
  std::vector<std::pair<std::string, int>> m_properties;

  std::atomic<cv::Mat*> m_latestFrame{nullptr};
  std::vector<cv::Mat*> m_sharedFreeList;
  wpi::spinlock m_sharedFreeListMutex;
  std::vector<cv::Mat*> m_sourceFreeList;
  std::atomic<bool> m_stopCamera{false};
  std::thread m_frameThread;

  wpi::gui::Texture m_tex;
};

void DisplayCameraSettings(CameraModel* model);

void DisplayCamera(CameraModel* model, const ImVec2& contentSize,
                   bool showFpsDataRate = true);

class CameraView : public View {
 public:
  explicit CameraView(CameraModel* model) : m_model{model} {}

  void Display() override;
  void Hidden() override;

  CameraModel* GetModel() { return m_model; }

 private:
  CameraModel* m_model;
};

}  // namespace glass
