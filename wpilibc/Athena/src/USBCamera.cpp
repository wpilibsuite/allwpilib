/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "USBCamera.h"

#include "Utility.h"

#include <regex>
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <iomanip>

// This macro expands the given imaq function to ensure that it is called and
// properly checked for an error, calling the wpi_setImaqErrorWithContext
// macro
// To call it, just give the name of the function and the arguments
#define SAFE_IMAQ_CALL(funName, ...)                \
  {                                                 \
    unsigned int error = funName(__VA_ARGS__);      \
    if (error != IMAQdxErrorSuccess)                \
      wpi_setImaqErrorWithContext(error, #funName); \
  }

/**
 * Helper function to determine the size of a jpeg. The general structure of
 * how to parse a jpeg for length can be found in this stackoverflow article:
 * http://stackoverflow.com/a/1602428. Be sure to also read the comments for
 * the SOS flag explanation.
 */
unsigned int USBCamera::GetJpegSize(void* buffer, unsigned int buffSize) {
  uint8_t* data = (uint8_t*)buffer;
  if (!wpi_assert(data[0] == 0xff && data[1] == 0xd8)) return 0;
  unsigned int pos = 2;
  while (pos < buffSize) {
    // All control markers start with 0xff, so if this isn't present,
    // the JPEG is not valid
    if (!wpi_assert(data[pos] == 0xff)) return 0;
    unsigned char t = data[pos + 1];
    // These are RST markers. We just skip them and move onto the next marker
    if (t == 0x01 || (t >= 0xd0 && t <= 0xd7)) {
      pos += 2;
    } else if (t == 0xd9) {
      // End of Image, add 2 for this and 0-indexed
      return pos + 2;
    } else if (!wpi_assert(t != 0xd8)) {
      // Another start of image, invalid image
      return 0;
    } else if (t == 0xda) {
      // SOS marker. The next two bytes are a 16-bit big-endian int that is
      // the length of the SOS header, skip that
      unsigned int len = (((unsigned int)(data[pos + 2] & 0xff)) << 8 |
                          ((unsigned int)data[pos + 3] & 0xff));
      pos += len + 2;
      // The next marker is the first marker that is 0xff followed by a non-RST
      // element. 0xff followed by 0x00 is an escaped 0xff. 0xd0-0xd7 are RST
      // markers
      while (data[pos] != 0xff || data[pos + 1] == 0x00 ||
             (data[pos + 1] >= 0xd0 && data[pos + 1] <= 0xd7)) {
        pos += 1;
        if (pos >= buffSize) return 0;
      }
    } else {
      // This is one of several possible markers. The next two bytes are a
      // 16-bit
      // big-endian int with the length of the marker header, skip that then
      // continue searching
      unsigned int len = (((unsigned int)(data[pos + 2] & 0xff)) << 8 |
                          ((unsigned int)data[pos + 3] & 0xff));
      pos += len + 2;
    }
  }

  return 0;
}

USBCamera::USBCamera(std::string name, bool useJpeg)
    : m_name(name),
      m_useJpeg(useJpeg) {}

void USBCamera::OpenCamera() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  for (unsigned int i = 0; i < 3; i++) {
    uInt32 id = 0;
    // Can't use SAFE_IMAQ_CALL here because we only error on the third time
    IMAQdxError error = IMAQdxOpenCamera(
        m_name.c_str(), IMAQdxCameraControlModeController, &id);
    if (error != IMAQdxErrorSuccess) {
      // Only error on the 3rd try
      if (i >= 2) wpi_setImaqErrorWithContext(error, "IMAQdxOpenCamera");
      // Sleep for a few seconds to ensure the error has been dealt with
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    } else {
      m_id = id;
      m_open = true;
      return;
    }
  }
}

void USBCamera::CloseCamera() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (!m_open) return;
  SAFE_IMAQ_CALL(IMAQdxCloseCamera, m_id);
  m_id = 0;
  m_open = false;
}

void USBCamera::StartCapture() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (!m_open || m_active) return;
  SAFE_IMAQ_CALL(IMAQdxConfigureGrab, m_id);
  SAFE_IMAQ_CALL(IMAQdxStartAcquisition, m_id);
  m_active = true;
}

void USBCamera::StopCapture() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (!m_open || !m_active) return;
  SAFE_IMAQ_CALL(IMAQdxStopAcquisition, m_id);
  SAFE_IMAQ_CALL(IMAQdxUnconfigureAcquisition, m_id);
  m_active = false;
}

void USBCamera::UpdateSettings() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  bool wasActive = m_active;

  if (wasActive) StopCapture();
  if (m_open) CloseCamera();
  OpenCamera();

  uInt32 count = 0;
  uInt32 currentMode = 0;
  SAFE_IMAQ_CALL(IMAQdxEnumerateVideoModes, m_id, nullptr, &count, &currentMode);
  auto modes = std::make_unique<IMAQdxVideoMode[]>(count);
  SAFE_IMAQ_CALL(IMAQdxEnumerateVideoModes, m_id, modes.get(), &count, &currentMode);

  // Groups are:
  //   0 - width
  //   1 - height
  //   2 - format
  //   3 - fps
  std::regex reMode("([0-9]+)\\s*x\\s*([0-9]+)\\s+(.*?)\\s+([0-9.]+)\\s*fps");
  IMAQdxVideoMode* foundMode = nullptr;
  IMAQdxVideoMode* currentModePtr = &modes[currentMode];
  double foundFps = 1000.0;

  // Loop through the modes, and find the match with the lowest fps
  for (unsigned int i = 0; i < count; i++) {
    std::cmatch m;
    if (!std::regex_match(modes[i].Name, m, reMode)) continue;
    unsigned int width = (unsigned int)std::stoul(m[1].str());
    unsigned int height = (unsigned int)std::stoul(m[2].str());
    if (width != m_width) continue;
    if (height != m_height) continue;
    double fps = atof(m[4].str().c_str());
    if (fps < m_fps) continue;
    if (fps > foundFps) continue;
    bool isJpeg =
        m[3].str().compare("jpeg") == 0 || m[3].str().compare("JPEG") == 0;
    if ((m_useJpeg && !isJpeg) || (!m_useJpeg && isJpeg)) continue;
    foundMode = &modes[i];
    foundFps = fps;
  }
  if (foundMode != nullptr) {
    if (foundMode->Value != currentModePtr->Value) {
      SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, IMAQdxAttributeVideoMode,
                     IMAQdxValueTypeU32, foundMode->Value);
    }
  }

  if (m_whiteBalance.compare(AUTO) == 0) {
    SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_WB_MODE,
                   IMAQdxValueTypeString, AUTO);
  } else {
    SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_WB_MODE,
                   IMAQdxValueTypeString, MANUAL);
    if (m_whiteBalanceValuePresent)
      SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_WB_VALUE,
                     IMAQdxValueTypeU32, m_whiteBalanceValue);
  }

  if (m_exposure.compare(AUTO) == 0) {
    SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_EX_MODE,
                   IMAQdxValueTypeString,
                   std::string("AutoAperaturePriority").c_str());
  } else {
    SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_EX_MODE,
                   IMAQdxValueTypeString, MANUAL);
    if (m_exposureValuePresent) {
      double minv = 0.0;
      double maxv = 0.0;
      SAFE_IMAQ_CALL(IMAQdxGetAttributeMinimum, m_id, ATTR_EX_VALUE,
                     IMAQdxValueTypeF64, &minv);
      SAFE_IMAQ_CALL(IMAQdxGetAttributeMaximum, m_id, ATTR_EX_VALUE,
                     IMAQdxValueTypeF64, &maxv);
      double val = minv + ((maxv - minv) * ((double)m_exposureValue / 100.0));
      SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_EX_VALUE,
                     IMAQdxValueTypeF64, val);
    }
  }

  SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_BR_MODE, IMAQdxValueTypeString,
                 MANUAL);
  double minv = 0.0;
  double maxv = 0.0;
  SAFE_IMAQ_CALL(IMAQdxGetAttributeMinimum, m_id, ATTR_BR_VALUE,
                 IMAQdxValueTypeF64, &minv);
  SAFE_IMAQ_CALL(IMAQdxGetAttributeMaximum, m_id, ATTR_BR_VALUE,
                 IMAQdxValueTypeF64, &maxv);
  double val = minv + ((maxv - minv) * ((double)m_brightness / 100.0));
  SAFE_IMAQ_CALL(IMAQdxSetAttribute, m_id, ATTR_BR_VALUE, IMAQdxValueTypeF64,
                 val);

  if (wasActive) StartCapture();
}

void USBCamera::SetFPS(double fps) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (m_fps != fps) {
    m_needSettingsUpdate = true;
    m_fps = fps;
  }
}

void USBCamera::SetSize(unsigned int width, unsigned int height) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (m_width != width || m_height != height) {
    m_needSettingsUpdate = true;
    m_width = width;
    m_height = height;
  }
}

void USBCamera::SetBrightness(unsigned int brightness) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (m_brightness != brightness) {
    m_needSettingsUpdate = true;
    m_brightness = brightness;
  }
}

unsigned int USBCamera::GetBrightness() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  return m_brightness;
}

void USBCamera::SetWhiteBalanceAuto() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  m_whiteBalance = AUTO;
  m_whiteBalanceValue = 0;
  m_whiteBalanceValuePresent = false;
  m_needSettingsUpdate = true;
}

void USBCamera::SetWhiteBalanceHoldCurrent() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  m_whiteBalance = MANUAL;
  m_whiteBalanceValue = 0;
  m_whiteBalanceValuePresent = false;
  m_needSettingsUpdate = true;
}

void USBCamera::SetWhiteBalanceManual(unsigned int whiteBalance) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  m_whiteBalance = MANUAL;
  m_whiteBalanceValue = whiteBalance;
  m_whiteBalanceValuePresent = true;
  m_needSettingsUpdate = true;
}

void USBCamera::SetExposureAuto() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  m_exposure = AUTO;
  m_exposureValue = 0;
  m_exposureValuePresent = false;
  m_needSettingsUpdate = true;
}

void USBCamera::SetExposureHoldCurrent() {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  m_exposure = MANUAL;
  m_exposureValue = 0;
  m_exposureValuePresent = false;
  m_needSettingsUpdate = true;
}

void USBCamera::SetExposureManual(unsigned int level) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  m_exposure = MANUAL;
  if (level > 100)
    m_exposureValue = 100;
  else
    m_exposureValue = level;
  m_exposureValuePresent = true;
  m_needSettingsUpdate = true;
}

void USBCamera::GetImage(Image* image) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (m_needSettingsUpdate || m_useJpeg) {
    m_needSettingsUpdate = false;
    m_useJpeg = false;
    UpdateSettings();
  }
  // BufNum is not actually used for anything at our level, since we are
  // waiting until the next image is ready anyway
  uInt32 bufNum;
  SAFE_IMAQ_CALL(IMAQdxGrab, m_id, image, 1, &bufNum);
}

unsigned int USBCamera::GetImageData(void* buffer, unsigned int bufferSize) {
  std::lock_guard<priority_recursive_mutex> lock(m_mutex);
  if (m_needSettingsUpdate || !m_useJpeg) {
    m_needSettingsUpdate = false;
    m_useJpeg = true;
    UpdateSettings();
  }
  // BufNum is not actually used for anything at our level
  uInt32 bufNum;
  SAFE_IMAQ_CALL(IMAQdxGetImageData, m_id, buffer, bufferSize,
                 IMAQdxBufferNumberModeLast, 0, &bufNum);
  return GetJpegSize(buffer, bufferSize);
}
