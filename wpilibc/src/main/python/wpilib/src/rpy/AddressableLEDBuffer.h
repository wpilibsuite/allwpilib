// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <stdexcept>
#include <vector>

#include "pybind11/pytypes.h"
#include "wpi/hardware/led/AddressableLED.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/Color8Bit.hpp"

namespace wpi {

/**
 * Buffer storage for Addressable LEDs.
 */
class AddressableLEDBuffer {
 public:
  /**
   * Constructs a new LED buffer with the specified length.
   *
   * @param length The length of the buffer in pixels
   */
  explicit AddressableLEDBuffer(size_t length) : m_buffer(length) {}

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index the index to write
   * @param r the r value [0-255]
   * @param g the g value [0-255]
   * @param b the b value [0-255]
   */
  void SetRGB(size_t index, int r, int g, int b);

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index the index to write
   * @param h the h value [0-180)
   * @param s the s value [0-255]
   * @param v the v value [0-255]
   */
  void SetHSV(size_t index, int h, int s, int v);

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index the index to write
   * @param color the color to write
   */
  void SetLED(size_t index, const wpi::util::Color& color);

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index the index to write
   * @param color the color to write
   */
  void SetLED(size_t index, const wpi::util::Color8Bit& color);

  /**
   * Gets the buffer length.
   *
   * @return the buffer length
   */
  size_t size() const { return m_buffer.size(); }

  /**
   * Gets the red value at the specified index.
   *
   * @param index the index
   * @return the red value
   */
  int GetRed(size_t index) const;

  /**
   * Gets the green value at the specified index.
   *
   * @param index the index
   * @return the green value
   */
  int GetGreen(size_t index) const;

  /**
   * Gets the blue value at the specified index.
   *
   * @param index the index
   * @return the blue value
   */
  int GetBlue(size_t index) const;

  /**
   * Gets the color at the specified index.
   *
   * @param index the index
   * @return the LED color
   */
  wpi::util::Color GetLED(size_t index) const;

  /**
   * Gets the color at the specified index.
   *
   * @param index the index
   * @return the LED color
   */
  wpi::util::Color8Bit GetLED8Bit(size_t index) const;

  /**
   * Implicit conversion to span of LED data
   */
  operator std::span<wpi::AddressableLED::LEDData>() {
    return std::span{m_buffer};
  }

  /**
   * Implicit conversion to span of const LED data
   */
  operator std::span<const wpi::AddressableLED::LEDData>() const {
    return std::span{m_buffer};
  }

  /**
   * Gets the LED data at the specified index.
   *
   * @param index the index
   * @return reference to the LED data
   */
  wpi::AddressableLED::LEDData& at(size_t index);

  /**
   * Gets the LED data at the specified index.
   *
   * @param index the index
   * @return reference to the LED data
   */
  wpi::AddressableLED::LEDData& operator[](size_t index);

  /**
   * Gets the LED data at the specified index.
   *
   * @param index the index
   * @return const reference to the LED data
   */
  const wpi::AddressableLED::LEDData& operator[](size_t index) const;

  auto begin() { return m_buffer.begin(); }
  auto end() { return m_buffer.end(); }

  /**
   * A view of another addressable LED buffer. Views provide an easy way to
   * split a large LED strip into smaller sections that can be animated
   * individually.
   */
  class View {
   public:
    /**
     * Gets the length of the view.
     */
    size_t size() const { return m_data.size(); }

    /**
     * Sets a specific LED in the view.
     *
     * @param index the index to write
     * @param r the r value [0-255]
     * @param g the g value [0-255]
     * @param b the b value [0-255]
     */
    void SetRGB(size_t index, int r, int g, int b);

    /**
     * Sets a specific LED in the view.
     *
     * @param index the index to write
     * @param h the h value [0-180)
     * @param s the s value [0-255]
     * @param v the v value [0-255]
     */
    void SetHSV(size_t index, int h, int s, int v);

    /**
     * Sets a specific LED in the view.
     *
     * @param index the index to write
     * @param color the color to write
     */
    void SetLED(size_t index, const wpi::util::Color& color);

    /**
     * Sets a specific LED in the view.
     *
     * @param index the index to write
     * @param color the color to write
     */
    void SetLED(size_t index, const wpi::util::Color8Bit& color);

    /**
     * Gets the LED data at the specified index.
     *
     * @param index the index
     * @return reference to the LED data
     */
    wpi::AddressableLED::LEDData& at(size_t index);

    /**
     * Gets the LED data at the specified index.
     *
     * @param index the index
     * @return reference to the LED data
     */
    wpi::AddressableLED::LEDData& operator[](size_t index);

    /**
     * Gets the LED data at the specified index.
     *
     * @param index the index
     * @return const reference to the LED data
     */
    const wpi::AddressableLED::LEDData& at(size_t index) const;

    /**
     * Gets the LED data at the specified index.
     *
     * @param index the index
     * @return const reference to the LED data
     */
    const wpi::AddressableLED::LEDData& operator[](size_t index) const;

    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }

    /**
     * Gets the color at the specified index.
     *
     * @param index the index
     * @return the LED color
     */
    wpi::util::Color GetLED(size_t index) const;

    /**
     * Gets the color at the specified index.
     *
     * @param index the index
     * @return the LED color
     */
    wpi::util::Color8Bit GetLED8Bit(size_t index) const;

    /**
     * Implicit conversion to span of LED data
     */
    operator std::span<wpi::AddressableLED::LEDData>() { return m_data; }

    /**
     * Implicit conversion to span of const LED data
     */
    operator std::span<const wpi::AddressableLED::LEDData>() const {
      return m_data;
    }

   private:
    friend class AddressableLEDBuffer;
    explicit View(std::span<wpi::AddressableLED::LEDData> data);

    std::span<wpi::AddressableLED::LEDData> m_data;
  };

  /**
   * Creates a read/write view of this buffer.
   *
   * @param slice the desired slice of the buffer (e.g. 2:4), step must be
   * unspecified or 1
   * @return View object representing the view
   * @throws std::out_of_range if the view would exceed buffer bounds
   */
  View CreateView(pybind11::slice slice);

 private:
  std::vector<wpi::AddressableLED::LEDData> m_buffer;
};

}  // namespace wpi
