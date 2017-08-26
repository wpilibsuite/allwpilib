/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>

#include <HAL/Types.h>

#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"
#include "tables/ITableListener.h"

namespace frc {

/**
 * Class implements the PWM generation in the FPGA.
 *
 * The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They
 * are mapped to the hardware dependent values, in this case 0-2000 for the
 * FPGA. Changes are immediately sent to the FPGA, and the update occurs at the
 * next FPGA cycle. There is no delay.
 *
 * As of revision 0.1.10 of the FPGA, the FPGA interprets the 0-2000 values as
 * follows:
 *   - 2000 = maximum pulse width
 *   - 1999 to 1001 = linear scaling from "full forward" to "center"
 *   - 1000 = center value
 *   - 999 to 2 = linear scaling from "center" to "full reverse"
 *   - 1 = minimum pulse width (currently .5ms)
 *   - 0 = disabled (i.e. PWM output is held low)
 */
class PWM : public SensorBase,
            public ITableListener,
            public LiveWindowSendable {
 public:
  enum PeriodMultiplier {
    kPeriodMultiplier_1X = 1,
    kPeriodMultiplier_2X = 2,
    kPeriodMultiplier_4X = 4
  };

  explicit PWM(int channel);
  virtual ~PWM();
  virtual void SetRaw(uint16_t value);
  virtual uint16_t GetRaw() const;
  virtual void SetPosition(double pos);
  virtual double GetPosition() const;
  virtual void SetSpeed(double speed);
  virtual double GetSpeed() const;
  virtual void SetDisabled();
  void SetPeriodMultiplier(PeriodMultiplier mult);
  void SetZeroLatch();
  void EnableDeadbandElimination(bool eliminateDeadband);
  void SetBounds(double max, double deadbandMax, double center,
                 double deadbandMin, double min);
  void SetRawBounds(int max, int deadbandMax, int center, int deadbandMin,
                    int min);
  void GetRawBounds(int32_t* max, int32_t* deadbandMax, int32_t* center,
                    int32_t* deadbandMin, int32_t* min);
  int GetChannel() const { return m_channel; }

 protected:
  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

  std::shared_ptr<ITable> m_table;

 private:
  int m_channel;
  HAL_DigitalHandle m_handle;
};

}  // namespace frc
