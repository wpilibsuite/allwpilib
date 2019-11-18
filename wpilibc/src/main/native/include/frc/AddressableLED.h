/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
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

namespace frc {
class PWM;

/**
 * A class for driving addressable LEDs, such as WS2812s and NeoPixels.
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
  };

  /**
   * Constructs a new driver from a PWM output.
   *
   * @param output the pwm output to use
   */
  explicit AddressableLED(PWM& output);

  /**
   * Constructs a new driver from a PWM output.
   *
   * @param output the pwm output to use
   */
  explicit AddressableLED(PWM* output);

  /**
   * Constructs a new driver from a PWM output.
   *
   * @param output the pwm output to use
   */
  explicit AddressableLED(std::shared_ptr<PWM> output);

  /**
   * Constructs a new driver for a specific port.
   *
   * @param port the output port to use (Must be a PWM port)
   */
  explicit AddressableLED(int port);

  ~AddressableLED() override;

  /**
   * Sets the length of the LED strip.
   *
   * <p>Calling this is an expensive call, so its best to call it once, then
   * just update data.
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
  void Init();

  std::shared_ptr<PWM> m_pwmOutput;
  hal::Handle<HAL_AddressableLEDHandle> m_handle;
};
}  // namespace frc
