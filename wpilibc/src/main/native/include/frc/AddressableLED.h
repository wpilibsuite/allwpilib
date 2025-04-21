// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>

#include <hal/AddressableLED.h>
#include <hal/AddressableLEDTypes.h>
#include <hal/PWM.h>
#include <hal/Types.h>
#include <units/time.h>

#include "util/Color.h"
#include "util/Color8Bit.h"

namespace frc {

/**
 * A class for driving addressable LEDs, such as WS2812B, WS2815, and NeoPixels.
 *
 * By default, the timing supports WS2812B and WS2815 LEDs, but is configurable
 * using SetBitTiming()
 *
 * Some LEDs use a different color order than the default GRB. The color order
 * is configurable using SetColorOrder().
 *
 * <p>Only 1 LED driver is currently supported by the roboRIO. However,
 * multiple LED strips can be connected in series and controlled from the
 * single driver.
 */
class AddressableLED {
 public:
  /**
   * Order that color data is sent over the wire.
   */
  enum ColorOrder {
    kRGB = HAL_ALED_RGB,  ///< RGB order
    kRBG = HAL_ALED_RBG,  ///< RBG order
    kBGR = HAL_ALED_BGR,  ///< BGR order
    kBRG = HAL_ALED_BRG,  ///< BRG order
    kGBR = HAL_ALED_GBR,  ///< GBR order
    kGRB = HAL_ALED_GRB   ///< GRB order. This is the default order.
  };

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

  AddressableLED(AddressableLED&&) = default;
  AddressableLED& operator=(AddressableLED&&) = default;

  /**
   * Sets the color order for this AddressableLED. The default order is GRB.
   *
   * This will take effect on the next call to SetData().
   *
   * @param order the color order
   */
  void SetColorOrder(ColorOrder order);

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
  void SetData(std::span<const LEDData> ledData);

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
   * <p>By default, the driver is set up to drive WS2812B and WS2815, so nothing
   * needs to be set for those.
   *
   * @param highTime0 high time for 0 bit (default 400ns)
   * @param lowTime0 low time for 0 bit (default 900ns)
   * @param highTime1 high time for 1 bit (default 900ns)
   * @param lowTime1 low time for 1 bit (default 600ns)
   */
  void SetBitTiming(units::nanosecond_t highTime0, units::nanosecond_t lowTime0,
                    units::nanosecond_t highTime1,
                    units::nanosecond_t lowTime1);

  /**
   * Sets the sync time.
   *
   * <p>The sync time is the time to hold output so LEDs enable. Default set for
   * WS2812B and WS2815.
   *
   * @param syncTime the sync time (default 280us)
   */
  void SetSyncTime(units::microsecond_t syncTime);

  /**
   * Starts the output.
   *
   * <p>The output writes continuously.
   */
  void Start();

  /**
   * Stops the output.
   */
  void Stop();

 private:
  hal::Handle<HAL_DigitalHandle, HAL_FreePWMPort> m_pwmHandle;
  hal::Handle<HAL_AddressableLEDHandle, HAL_FreeAddressableLED> m_handle;
  int m_port;
};

constexpr auto format_as(AddressableLED::ColorOrder order) {
  return static_cast<int32_t>(order);
}

}  // namespace frc
