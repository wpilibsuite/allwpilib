/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.vision;

import java.nio.ByteBuffer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.ni.vision.NIVision;
import com.ni.vision.VisionException;

import static com.ni.vision.NIVision.Image;
import static edu.wpi.first.wpilibj.Timer.delay;

public class USBCamera {
  public static String kDefaultCameraName = "cam0";

  private static String ATTR_VIDEO_MODE = "AcquisitionAttributes::VideoMode";
  private static String ATTR_WB_MODE = "CameraAttributes::WhiteBalance::Mode";
  private static String ATTR_WB_VALUE = "CameraAttributes::WhiteBalance::Value";
  private static String ATTR_EX_MODE = "CameraAttributes::Exposure::Mode";
  private static String ATTR_EX_VALUE = "CameraAttributes::Exposure::Value";
  private static String ATTR_BR_MODE = "CameraAttributes::Brightness::Mode";
  private static String ATTR_BR_VALUE = "CameraAttributes::Brightness::Value";

  public class WhiteBalance {
    public static final int kFixedIndoor = 3000;
    public static final int kFixedOutdoor1 = 4000;
    public static final int kFixedOutdoor2 = 5000;
    public static final int kFixedFluorescent1 = 5100;
    public static final int kFixedFlourescent2 = 5200;
  }

  private Pattern m_reMode =
      Pattern
          .compile("(?<width>[0-9]+)\\s*x\\s*(?<height>[0-9]+)\\s+(?<format>.*?)\\s+(?<fps>[0-9.]+)\\s*fps");

  private String m_name = kDefaultCameraName;
  private int m_id = -1;
  private boolean m_active = false;
  private boolean m_useJpeg = true;
  private int m_width = 320;
  private int m_height = 240;
  private int m_fps = 30;
  private String m_whiteBalance = "auto";
  private int m_whiteBalanceValue = -1;
  private String m_exposure = "auto";
  private int m_exposureValue = -1;
  private int m_brightness = 50;
  private boolean m_needSettingsUpdate = true;

  public USBCamera() {
    openCamera();
  }

  public USBCamera(String name) {
    m_name = name;
    openCamera();
  }

  public synchronized void openCamera() {
    if (m_id != -1)
      return; // Camera is already open
    for (int i = 0; i < 3; i++) {
      try {
        m_id =
            NIVision.IMAQdxOpenCamera(m_name,
                NIVision.IMAQdxCameraControlMode.CameraControlModeController);
      } catch (VisionException e) {
        if (i == 2)
          throw e;
        delay(2.0);
        continue;
      }
      break;
    }
  }

  public synchronized void closeCamera() {
    if (m_id == -1)
      return;
    NIVision.IMAQdxCloseCamera(m_id);
    m_id = -1;
  }

  public synchronized void startCapture() {
    if (m_id == -1 || m_active)
      return;
    NIVision.IMAQdxConfigureGrab(m_id);
    NIVision.IMAQdxStartAcquisition(m_id);
    m_active = true;
  }

  public synchronized void stopCapture() {
    if (m_id == -1 || !m_active)
      return;
    NIVision.IMAQdxStopAcquisition(m_id);
    NIVision.IMAQdxUnconfigureAcquisition(m_id);
    m_active = false;
  }

  public synchronized void updateSettings() {
    boolean wasActive = m_active;
    // Stop acquistion, close and reopen camera
    if (wasActive)
      stopCapture();
    if (m_id != -1)
      closeCamera();
    openCamera();

    // Video Mode
    NIVision.dxEnumerateVideoModesResult enumerated = NIVision.IMAQdxEnumerateVideoModes(m_id);
    NIVision.IMAQdxEnumItem foundMode = null;
    int foundFps = 1000;
    for (NIVision.IMAQdxEnumItem mode : enumerated.videoModeArray) {
      Matcher m = m_reMode.matcher(mode.Name);
      if (!m.matches())
        continue;
      if (Integer.parseInt(m.group("width")) != m_width)
        continue;
      if (Integer.parseInt(m.group("height")) != m_height)
        continue;
      double fps = Double.parseDouble(m.group("fps"));
      if (fps < m_fps)
        continue;
      if (fps > foundFps)
        continue;
      String format = m.group("format");
      boolean isJpeg = format.equals("jpeg") || format.equals("JPEG");
      if ((m_useJpeg && !isJpeg) || (!m_useJpeg && isJpeg))
        continue;
      foundMode = mode;
      foundFps = (int) fps;
    }
    if (foundMode != null) {
      System.out.println("found mode " + foundMode.Value + ": " + foundMode.Name);
      if (foundMode.Value != enumerated.currentMode)
        NIVision.IMAQdxSetAttributeU32(m_id, ATTR_VIDEO_MODE, foundMode.Value);
    }

    // White Balance
    if (m_whiteBalance == "auto")
      NIVision.IMAQdxSetAttributeString(m_id, ATTR_WB_MODE, "Auto");
    else {
      NIVision.IMAQdxSetAttributeString(m_id, ATTR_WB_MODE, "Manual");
      if (m_whiteBalanceValue != -1)
        NIVision.IMAQdxSetAttributeI64(m_id, ATTR_WB_VALUE, m_whiteBalanceValue);
    }

    // Exposure
    if (m_exposure == "auto")
      NIVision.IMAQdxSetAttributeString(m_id, ATTR_EX_MODE, "AutoAperaturePriority");
    else {
      NIVision.IMAQdxSetAttributeString(m_id, ATTR_EX_MODE, "Manual");
      if (m_exposureValue != -1) {
        long minv = NIVision.IMAQdxGetAttributeMinimumI64(m_id, ATTR_EX_VALUE);
        long maxv = NIVision.IMAQdxGetAttributeMaximumI64(m_id, ATTR_EX_VALUE);
        long val = minv + (long) (((double) (maxv - minv)) * (((double) m_exposureValue) / 100.0));
        NIVision.IMAQdxSetAttributeI64(m_id, ATTR_EX_VALUE, val);
      }
    }

    // Brightness
    NIVision.IMAQdxSetAttributeString(m_id, ATTR_BR_MODE, "Manual");
    long minv = NIVision.IMAQdxGetAttributeMinimumI64(m_id, ATTR_BR_VALUE);
    long maxv = NIVision.IMAQdxGetAttributeMaximumI64(m_id, ATTR_BR_VALUE);
    long val = minv + (long) (((double) (maxv - minv)) * (((double) m_brightness) / 100.0));
    NIVision.IMAQdxSetAttributeI64(m_id, ATTR_BR_VALUE, val);

    // Restart acquisition
    if (wasActive)
      startCapture();
  }

  public synchronized void setFPS(int fps) {
    if (fps != m_fps) {
      m_needSettingsUpdate = true;
      m_fps = fps;
    }
  }

  public synchronized void setSize(int width, int height) {
    if (width != m_width || height != m_height) {
      m_needSettingsUpdate = true;
      m_width = width;
      m_height = height;
    }
  }

  /** Set the brightness, as a percentage (0-100). */
  public synchronized void setBrightness(int brightness) {
    if (brightness > 100)
      m_brightness = 100;
    else if (brightness < 0)
      m_brightness = 0;
    else
      m_brightness = brightness;
    m_needSettingsUpdate = true;
  }

  /** Get the brightness, as a percentage (0-100). */
  public synchronized int getBrightness() {
    return m_brightness;
  }

  /** Set the white balance to auto. */
  public synchronized void setWhiteBalanceAuto() {
    m_whiteBalance = "auto";
    m_whiteBalanceValue = -1;
    m_needSettingsUpdate = true;
  }

  /** Set the white balance to hold current. */
  public synchronized void setWhiteBalanceHoldCurrent() {
    m_whiteBalance = "manual";
    m_whiteBalanceValue = -1;
    m_needSettingsUpdate = true;
  }

  /** Set the white balance to manual, with specified color temperature. */
  public synchronized void setWhiteBalanceManual(int value) {
    m_whiteBalance = "manual";
    m_whiteBalanceValue = value;
    m_needSettingsUpdate = true;
  }

  /** Set the exposure to auto aperature. */
  public synchronized void setExposureAuto() {
    m_exposure = "auto";
    m_exposureValue = -1;
    m_needSettingsUpdate = true;
  }

  /** Set the exposure to hold current. */
  public synchronized void setExposureHoldCurrent() {
    m_exposure = "manual";
    m_exposureValue = -1;
    m_needSettingsUpdate = true;
  }

  /** Set the exposure to manual, as a percentage (0-100). */
  public synchronized void setExposureManual(int value) {
    m_exposure = "manual";
    if (value > 100)
      m_exposureValue = 100;
    else if (value < 0)
      m_exposureValue = 0;
    else
      m_exposureValue = value;
    m_needSettingsUpdate = true;
  }

  public synchronized void getImage(Image image) {
    if (m_needSettingsUpdate || m_useJpeg) {
      m_needSettingsUpdate = false;
      m_useJpeg = false;
      updateSettings();
    }

    NIVision.IMAQdxGrab(m_id, image, 1);
  }

  public synchronized void getImageData(ByteBuffer data) {
    if (m_needSettingsUpdate || !m_useJpeg) {
      m_needSettingsUpdate = false;
      m_useJpeg = true;
      updateSettings();
    }

    NIVision
        .IMAQdxGetImageData(m_id, data, NIVision.IMAQdxBufferNumberMode.BufferNumberModeLast, 0);
    data.limit(data.capacity() - 1);
    data.limit(getJpegSize(data));
  }

  private static int getJpegSize(ByteBuffer data) {
    if (data.get(0) != (byte) 0xff || data.get(1) != (byte) 0xd8)
      throw new VisionException("invalid image");
    int pos = 2;
    while (true) {
      try {
        byte b = data.get(pos);
        if (b != (byte) 0xff)
          throw new VisionException("invalid image at pos " + pos + " (" + data.get(pos) + ")");
        b = data.get(pos + 1);
        if (b == (byte) 0x01 || (b >= (byte) 0xd0 && b <= (byte) 0xd7)) // various
          pos += 2;
        else if (b == (byte) 0xd9) // EOI
          return pos + 2;
        else if (b == (byte) 0xd8) // SOI
          throw new VisionException("invalid image");
        else if (b == (byte) 0xda) { // SOS
          int len = ((data.get(pos + 2) & 0xff) << 8) | (data.get(pos + 3) & 0xff);
          pos += len + 2;
          // Find next marker. Skip over escaped and RST markers.
          while (data.get(pos) != (byte) 0xff || data.get(pos + 1) == (byte) 0x00
              || (data.get(pos + 1) >= (byte) 0xd0 && data.get(pos + 1) <= (byte) 0xd7))
            pos += 1;
        } else { // various
          int len = ((data.get(pos + 2) & 0xff) << 8) | (data.get(pos + 3) & 0xff);
          pos += len + 2;
        }
      } catch (IndexOutOfBoundsException ex) {
        throw new VisionException("invalid image: could not find jpeg end " + ex.getMessage());
      }
    }
  }
}
