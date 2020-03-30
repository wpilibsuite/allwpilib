/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/AddressableLEDTypes.h>
#include <hal/Types.h>
#include <units/units.h>
#include <wpi/ArrayRef.h>

#include "frc/ErrorBase.h"
#include "util/Color.h"
#include "util/Color8Bit.h"

namespace frc {

/**
 * A class for driving addressable LEDs, such as WS2812s and NeoPixels.
 *
 * <p>Only 1 LED driver is currently supported by the roboRIO.
 */
class AddressableLED : public ErrorBase {
 public:
  class LEDData : public HAL_AddressableLEDData {
   public:
    LEDData() : LEDData(0, 0, 0) {}
    LEDData(int _r, int _g, int _b) {
      r = _r;
      g = _g;
      b = _b;
      padding = 0;
    }

    /**
     * A helper method to set all values of the LED.
     *
     * @param r the r value [0-255]
     * @param g the g value [0-255]
     * @param b the b value [0-255]
     */
    void SetRGB(int r, int g, int b) {
      this->r = r;
      this->g = g;
      this->b = b;
    }

    /**
     * A helper method to set all values of the LED.
     *
     * @param h the h value [0-180]
     * @param s the s value [0-255]
     * @param v the v value [0-255]
     */
    void SetHSV(int h, int s, int v);

    /*
     * Sets a specific LED in the buffer.
     *
     * @param color The color of the LED
     */
    void SetLED(const Color& color) {
      this->r = color.red * 255;
      this->g = color.green * 255;
      this->b = color.blue * 255;
    }

    /*
     * Sets a specific LED in the buffer.
     *
     * @param color The color of the LED
     */
    void SetLED(const Color8Bit& color) {
      this->r = color.red;
      this->g = color.green;
      this->b = color.blue;
    }
  };

  /**
   * Constructs a new driver for a specific port.
   *
   * @param port the output port to use (Must be a PWM header)
   */
  explicit AddressableLED(int port);

  ~AddressableLED() override;

  /**
   * Sets the length of the LED strip.
   *
   * <p>Calling this is an expensive call, so its best to call it once, then
   * just update data.
   *
   * <p>The max length is 5460 LEDs.
   *
   * @param length the strip length
   */
  void SetLength(int length);

  /**
   * Sets the led output data.
   *
   * <p>If the output is enabled, this will start writing the next data cycle.
   * It is safe to call, even while output is enabled.
   *
   * @param ledData the buffer to write
   */
  void SetData(wpi::ArrayRef<LEDData> ledData);

  /**
   * Sets the led output data.
   *
   * <p>If the output is enabled, this will start writing the next data cycle.
   * It is safe to call, even while output is enabled.
   *
   * @param ledData the buffer to write
   */
  void SetData(std::initializer_list<LEDData> ledData);

  /**
   * Sets the bit timing.
   *
   * <p>By default, the driver is set up to drive WS2812s, so nothing needs to
   * be set for those.
   *
   * @param lowTime0 low time for 0 bit
   * @param highTime0 high time for 0 bit
   * @param lowTime1 low time for 1 bit
   * @param highTime1 high time for 1 bit
   */
  void SetBitTiming(units::nanosecond_t lowTime0, units::nanosecond_t highTime0,
                    units::nanosecond_t lowTime1,
                    units::nanosecond_t highTime1);

  /**
   * Sets the sync time.
   *
   * <p>The sync time is the time to hold output so LEDs enable. Default set for
   * WS2812.
   *
   * @param syncTimeMicroSeconds the sync time
   */
  void SetSyncTime(units::microsecond_t syncTime);

  /**
   * Starts the output.
   *
   * <p>The output writes continously.
   */
  void Start();

  /**
   * Stops the output.
   */
  void Stop();

 private:
  hal::Handle<HAL_DigitalHandle> m_pwmHandle;
  hal::Handle<HAL_AddressableLEDHandle> m_handle;
};
}  // namespace frc
