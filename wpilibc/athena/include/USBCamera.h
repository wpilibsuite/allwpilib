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

  int m_width = 320;
  int m_height = 240;
  double m_fps = 30;
  std::string m_whiteBalance = AUTO;
  int m_whiteBalanceValue = 0;
  bool m_whiteBalanceValuePresent = false;
  std::string m_exposure = MANUAL;
  int m_exposureValue = 50;
  bool m_exposureValuePresent = false;
  int m_brightness = 80;
  bool m_needSettingsUpdate = true;

  int GetJpegSize(void* buffer, int buffSize);

 public:
  static constexpr char const* kDefaultCameraName = "cam0";

  USBCamera(std::string name, bool useJpeg);

  void OpenCamera();
  void CloseCamera();
  void StartCapture();
  void StopCapture();
  void SetFPS(double fps);
  void SetSize(int width, int height);

  void UpdateSettings();
  /**
   * Set the brightness, as a percentage (0-100).
   */
  void SetBrightness(int brightness);

  /**
   * Get the brightness, as a percentage (0-100).
   */
  int GetBrightness();

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
  void SetWhiteBalanceManual(int wbValue);

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
  void SetExposureManual(int expValue);

  void GetImage(Image* image);
  int GetImageData(void* buffer, int bufferSize);
};
