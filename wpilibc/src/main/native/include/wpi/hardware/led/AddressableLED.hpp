// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>

#include "wpi/hal/AddressableLED.h"
#include "wpi/hal/AddressableLEDTypes.h"
#include "wpi/hal/Types.h"
#include "wpi/units/time.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/Color8Bit.hpp"

namespace frc {

/**
 * A class for driving addressable LEDs, such as WS2812B, WS2815, and NeoPixels.
 *
 * Some LEDs use a different color order than the default GRB. The color order
 * is configurable using SetColorOrder().
 *
 * Up to 1024 LEDs may be controlled in total across all AddressableLED
 * instances. A single global buffer is used for all instances. The start
 * position used for LED data for the output is set via SetStart() and the
 * length of the strip is set via SetLength(). Both of these default to zero, so
 * multiple instances will access the same pixel data unless SetStart() is
 * called to adjust the starting point.
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
   * Constructs a new driver for a specific channel.
   *
   * @param channel the output channel to use
   */
  explicit AddressableLED(int channel);

  AddressableLED(AddressableLED&&) = default;
  AddressableLED& operator=(AddressableLED&&) = default;

  /**
   * Gets the channel for this addressable LED.
   *
   * @return channel
   */
  int GetChannel() const { return m_channel; }

  /**
   * Sets the color order for this AddressableLED. The default order is GRB.
   *
   * This will take effect on the next call to SetData().
   *
   * @param order the color order
   */
  void SetColorOrder(ColorOrder order);

  /**
   * Sets the display start of the LED strip in the global buffer.
   *
   * @param start the strip start, in LEDs
   */
  void SetStart(int start);

  /**
   * Gets the display start of the LED strip in the global buffer.
   *
   * @return the strip start, in LEDs
   */
  int GetStart() const { return m_start; }

  /**
   * Sets the length of the LED strip.
   *
   * @param length the strip length, in LEDs
   */
  void SetLength(int length);

  /**
   * Sets the LED output data. This will write to the global buffer starting at
   * the location set by SetStart() and up to the length set by SetLength().
   *
   * @param ledData the buffer to write
   */
  void SetData(std::span<const LEDData> ledData);

  /**
   * Sets the LED output data. This will write to the global buffer starting at
   * the location set by SetStart() and up to the length set by SetLength().
   *
   * @param ledData the buffer to write
   */
  void SetData(std::initializer_list<LEDData> ledData);

  /**
   * Sets the LED output data at an arbitrary location in the global buffer.
   *
   * @param start the start location, in LEDs
   * @param colorOrder the color order
   * @param ledData the buffer to write
   */
  static void SetGlobalData(int start, ColorOrder colorOrder,
                            std::span<const LEDData> ledData);

 private:
  hal::Handle<HAL_AddressableLEDHandle, HAL_FreeAddressableLED> m_handle;
  int m_channel;
  int m_start{0};
  int m_length{0};
  ColorOrder m_colorOrder{kGRB};
};

constexpr auto format_as(AddressableLED::ColorOrder order) {
  return static_cast<int32_t>(order);
}

}  // namespace frc
