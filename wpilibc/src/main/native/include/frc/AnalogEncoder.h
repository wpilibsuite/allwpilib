/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <hal/SimDevice.h>
#include <hal/Types.h>
#include <units/units.h>

#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"
#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {
class AnalogInput;

/**
 * Class for supporting continuous analog encoders, such as the US Digital MA3.
 */
class AnalogEncoder : public ErrorBase,
                      public Sendable,
                      public SendableHelper<AnalogEncoder> {
 public:
  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(AnalogInput& analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(AnalogInput* analogInput);

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   *
   * @param analogInput the analog input to attach to
   */
  explicit AnalogEncoder(std::shared_ptr<AnalogInput> analogInput);

  ~AnalogEncoder() override = default;

  AnalogEncoder(AnalogEncoder&&) = default;
  AnalogEncoder& operator=(AnalogEncoder&&) = default;

  /**
   * Reset the Encoder distance to zero.
   */
  void Reset();

  /**
   * Get the encoder value since the last reset.
   *
   * This is reported in rotations since the last reset.
   *
   * @return the encoder value in rotations
   */
  units::turn_t Get() const;

  /**
   * Get the offset of position relative to the last reset.
   *
   * GetPositionInRotation() - GetPositionOffset() will give an encoder absolute
   * position relative to the last reset. This could potentially be negative,
   * which needs to be accounted for.
   *
   * @return the position offset
   */
  double GetPositionOffset() const;

  /**
   * Set the distance per rotation of the encoder. This sets the multiplier used
   * to determine the distance driven based on the rotation value from the
   * encoder. Set this value based on the how far the mechanism travels in 1
   * rotation of the encoder, and factor in gearing reductions following the
   * encoder shaft. This distance can be in any units you like, linear or
   * angular.
   *
   * @param distancePerRotation the distance per rotation of the encoder
   */
  void SetDistancePerRotation(double distancePerRotation);

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful
   * units.
   */
  double GetDistancePerRotation() const;

  /**
   * Get the distance the sensor has driven since the last reset as scaled by
   * the value from SetDistancePerRotation.
   *
   * @return The distance driven since the last reset
   */
  double GetDistance() const;

  void InitSendable(SendableBuilder& builder) override;

 private:
  void Init();

  std::shared_ptr<AnalogInput> m_analogInput;
  AnalogTrigger m_analogTrigger;
  Counter m_counter;
  double m_positionOffset = 0;
  double m_distancePerRotation = 1.0;
  mutable units::turn_t m_lastPosition{0.0};

  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;
};
}  // namespace frc
