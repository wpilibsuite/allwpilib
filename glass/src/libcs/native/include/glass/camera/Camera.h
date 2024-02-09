// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include "cscore_c.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <cscore_cpp.h>
#include <cscore_cv.h>
#include <wpi/mutex.h>
#include <wpi/spinlock.h>
#include <wpigui.h>

#include "glass/Model.h"
#include "glass/Storage.h"
#include "glass/View.h"

namespace glass {

class Storage;

class CameraModel : public Model {
 public:
  explicit CameraModel(Storage& storage, std::string_view id);
  ~CameraModel() override;

  CameraModel(const CameraModel&) = delete;
  CameraModel& operator=(const CameraModel&) = delete;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

  void Start();
  void Stop();

  const std::string& GetId() { return m_id; }

  wpi::gui::Texture& GetTexture() { return m_tex; }

  /**
   * Set the source handle.
   */
  void SetSource(cs::VideoSource source);

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

  /**
   * For a HTTP camera, change the URLs used to connect to the camera.
   */
  void SetUrls(std::span<const std::string> urls);

  cs::VideoSource::Kind GetKind() const { return m_source.GetKind(); }

  cs::VideoMode GetVideoMode() const { return m_source.GetVideoMode(); }
  void SetVideoMode(const cs::VideoMode& mode);
  void ResetVideoMode();

 private:
  cv::Mat* AllocMat();

  std::string m_id;
  cs::VideoSource m_source;

  std::string& m_cameraType;
  std::string& m_usbPath;
  std::vector<std::string>& m_urls;
  std::string& m_pixelFormatStr;
  int& m_width;
  int& m_height;
  int& m_fps;

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

void DisplayCameraWindow(CameraModel* model);

inline CameraModel* GetOrNewCameraModel(Storage& root, std::string_view id) {
  Storage& storage = root.GetChild(id);
  return &storage.GetOrNewData<CameraModel>(storage, id);
}

inline CameraModel* GetCameraModel(Storage& root, std::string_view id) {
  return root.GetChild(id).GetData<CameraModel>();
}

void DisplayCameraModelTable(Storage& root,
                             int kinds = CS_SOURCE_USB | CS_SOURCE_HTTP |
                                         CS_SOURCE_CV | CS_SOURCE_RAW);

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
