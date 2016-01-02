/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "USBCamera.h"
#include "ErrorBase.h"
#include "nivision.h"
#include "NIIMAQdx.h"

#include "HAL/cpp/priority_mutex.h"
#include <thread>
#include <memory>
#include <condition_variable>
#include <tuple>
#include <vector>

class CameraServer : public ErrorBase {
 private:
  static constexpr uint16_t kPort = 1180;
  static constexpr uint8_t kMagicNumber[] = {0x01, 0x00, 0x00, 0x00};
  static constexpr uint32_t kSize640x480 = 0;
  static constexpr uint32_t kSize320x240 = 1;
  static constexpr uint32_t kSize160x120 = 2;
  static constexpr int32_t kHardwareCompression = -1;
  static constexpr uint32_t kMaxImageSize = 200000;

 protected:
  CameraServer();

  std::shared_ptr<USBCamera> m_camera;
  std::thread m_serverThread;
  std::thread m_captureThread;
  priority_recursive_mutex m_imageMutex;
  std::condition_variable_any m_newImageVariable;
  std::vector<uint8_t*> m_dataPool;
  unsigned int m_quality;
  bool m_autoCaptureStarted;
  bool m_hwClient;
  std::tuple<uint8_t*, unsigned int, unsigned int, bool> m_imageData;

  void Serve();
  void AutoCapture();
  void SetImageData(uint8_t* data, unsigned int size, unsigned int start = 0,
                    bool imaqData = false);
  void FreeImageData(
      std::tuple<uint8_t*, unsigned int, unsigned int, bool> imageData);

  struct Request {
    uint32_t fps;
    int32_t compression;
    uint32_t size;
  };

 public:
  static CameraServer* GetInstance();
  void SetImage(Image const* image);

  void StartAutomaticCapture(
      char const* cameraName = USBCamera::kDefaultCameraName);

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * You should call this method to just see a camera feed on the
   * dashboard without doing any vision processing on the roboRIO.
   * {@link #SetImage} should not be called after this is called.
   *
   * @param camera The camera interface (eg. USBCamera)
   */
  void StartAutomaticCapture(std::shared_ptr<USBCamera> camera);

  bool IsAutoCaptureStarted();

  void SetQuality(unsigned int quality);
  unsigned int GetQuality();

  void SetSize(unsigned int size);
};
