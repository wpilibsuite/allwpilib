/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Base.h"
#include "Controller.h"
#include "LiveWindow/LiveWindow.h"
#include "HAL/Semaphore.hpp"
#include "PIDInterface.h"

class PIDOutput;
class PIDSource;
class Notifier;

/**
 * Class implements a PID Control Loop.
 *
 * Creates a separate thread which reads the given PIDSource and takes
 * care of the integral calculations, as well as writing the given
 * PIDOutput
 */
class PIDController : public LiveWindowSendable,
                      public PIDInterface,
                      public ITableListener {
 public:
  PIDController(float p, float i, float d, PIDSource *source, PIDOutput *output,
                float period = 0.05);
  PIDController(float p, float i, float d, float f, PIDSource *source,
                PIDOutput *output, float period = 0.05);
  virtual ~PIDController();
  virtual float Get() const;
  virtual void SetContinuous(bool continuous = true);
  virtual void SetInputRange(float minimumInput, float maximumInput);
  virtual void SetOutputRange(float minimumOutput, float maximumOutput);
  virtual void SetPID(double p, double i, double d) override;
  virtual void SetPID(double p, double i, double d, double f);
  virtual double GetP() const override;
  virtual double GetI() const override;
  virtual double GetD() const override;
  virtual double GetF() const;

  virtual void SetSetpoint(float setpoint) override;
  virtual double GetSetpoint() const override;

  virtual float GetError() const;

  virtual void SetTolerance(float percent);
  virtual void SetAbsoluteTolerance(float absValue);
  virtual void SetPercentTolerance(float percentValue);
  virtual bool OnTarget() const;

  virtual void Enable() override;
  virtual void Disable() override;
  virtual bool IsEnabled() const override;

  virtual void Reset() override;

  virtual void InitTable(ITable *table);

 private:
  float m_P;              // factor for "proportional" control
  float m_I;              // factor for "integral" control
  float m_D;              // factor for "derivative" control
  float m_F;              // factor for "feed forward" control
  float m_maximumOutput;  // |maximum output|
  float m_minimumOutput;  // |minimum output|
  float m_maximumInput;   // maximum input - limit setpoint to this
  float m_minimumInput;   // minimum input - limit setpoint to this
  bool m_continuous;      // do the endpoints wrap around? eg. Absolute encoder
  bool m_enabled;  // is the pid controller enabled
  bool m_destruct;    // should the calculate thread stop running
  float m_prevError;  // the prior sensor input (used to compute velocity)
  double m_totalError;  // the sum of the errors for use in the integral calc
  enum { kAbsoluteTolerance, kPercentTolerance, kNoTolerance } m_toleranceType;
  float m_tolerance;  // the percetage or absolute error that is considered on
                      // target
  float m_setpoint;
  float m_error;
  float m_result;
  float m_period;

  MUTEX_ID m_semaphore;

  PIDSource *m_pidInput;
  PIDOutput *m_pidOutput;

  Notifier *m_controlLoop;

  void Initialize(float p, float i, float d, float f, PIDSource *source,
                  PIDOutput *output, float period = 0.05);
  static void CallCalculate(void *controller);

  virtual ITable *GetTable() const;
  virtual std::string GetSmartDashboardType() const;
  virtual void ValueChanged(ITable *source, const std::string &key,
                            EntryValue value, bool isNew);
  virtual void UpdateTable();
  virtual void StartLiveWindowMode();
  virtual void StopLiveWindowMode();

 protected:
  ITable *m_table;
  virtual void Calculate();

  DISALLOW_COPY_AND_ASSIGN(PIDController);
};
