/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include "ErrorBase.h"
#include "HAL/cpp/priority_mutex.h"
#include "NIIMAQdx.h"
#include "nivision.h"

typedef enum whiteBalance_enum {
  kFixedIndoor = 3000,
  kFixedOutdoor1 = 4000,
  kFixedOutdoor2 = 5000,
  kFixedFluorescent1 = 5100,
  kFixedFlourescent2 = 5200
} whiteBalance;

class USBCamera : public ErrorBase {
 private:
  static constexpr char const* ATTR_WB_MODE =
      "CameraAttributes::WhiteBalance::Mode";
  static constexpr char const* ATTR_WB_VALUE =
      "CameraAttributes::WhiteBalance::Value";
  static constexpr char const* ATTR_EX_MODE =
      "CameraAttributes::Exposure::Mode";
  static constexpr char const* ATTR_EX_VALUE =
      "CameraAttributes::Exposure::Value";
  static constexpr char const* ATTR_BR_MODE =
      "CameraAttributes::Brightness::Mode";
  static constexpr char const* ATTR_BR_VALUE =
      "CameraAttributes::Brightness::Value";

  // Constants for the manual and auto types
  static constexpr char const* AUTO = "Auto";
  static constexpr char const* MANUAL = "Manual";

 protected:
  IMAQdxSession m_id = 0;
  std::string m_name;
  bool m_useJpeg;
  bool m_active = false;
  bool m_open = false;

  priority_recursive_mutex m_mutex;

  uint32_t m_width = 320;
  uint32_t m_height = 240;
  double m_fps = 30;
  std::string m_whiteBalance = AUTO;
  uint32_t m_whiteBalanceValue = 0;
  bool m_whiteBalanceValuePresent = false;
  std::string m_exposure = MANUAL;
  uint32_t m_exposureValue = 50;
  bool m_exposureValuePresent = false;
  uint32_t m_brightness = 80;
  bool m_needSettingsUpdate = true;

  uint32_t GetJpegSize(void* buffer, uint32_t buffSize);

 public:
  static constexpr char const* kDefaultCameraName = "cam0";

  USBCamera(std::string name, bool useJpeg);

  void OpenCamera();
  void CloseCamera();
  void StartCapture();
  void StopCapture();
  void SetFPS(double fps);
  void SetSize(uint32_t width, uint32_t height);

  void UpdateSettings();
  /**
   * Set the brightness, as a percentage (0-100).
   */
  void SetBrightness(uint32_t brightness);

  /**
   * Get the brightness, as a percentage (0-100).
   */
  uint32_t GetBrightness();

  /**
   * Set the white balance to auto
   */
  void SetWhiteBalanceAuto();

  /**
   * Set the white balance to hold current
   */
  void SetWhiteBalanceHoldCurrent();

  /**
   * Set the white balance to manual, with specified color temperature
   */
  void SetWhiteBalanceManual(uint32_t wbValue);

  /**
   * Set the exposure to auto exposure
   */
  void SetExposureAuto();

  /**
   * Set the exposure to hold current
   */
  void SetExposureHoldCurrent();

  /**
   * Set the exposure to manual, with a given percentage (0-100)
   */
  void SetExposureManual(uint32_t expValue);

  void GetImage(Image* image);
  uint32_t GetImageData(void* buffer, uint32_t bufferSize);
};
