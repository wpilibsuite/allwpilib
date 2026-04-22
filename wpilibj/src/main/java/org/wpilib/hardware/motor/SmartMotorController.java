// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;


import static org.wpilib.units.Units.Meters;
import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.Milliseconds;
import static org.wpilib.units.Units.Radians;
import static org.wpilib.units.Units.RadiansPerSecond;
import static org.wpilib.units.Units.Rotations;
import static org.wpilib.units.Units.RotationsPerSecond;
import static org.wpilib.units.Units.Seconds;
import static org.wpilib.units.Units.Volts;

import java.util.Optional;
import java.util.OptionalDouble;
import java.util.concurrent.atomic.AtomicReference;
import org.wpilib.hardware.motor.SmartMotorControllerConfig.ControlMode;
import org.wpilib.hardware.motor.SmartMotorControllerConfig.IdleMode;
import org.wpilib.math.controller.ArmFeedforward;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.trajectory.ExponentialProfile;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.math.trajectory.TrapezoidProfile.State;
import org.wpilib.networktables.NetworkTable;
import org.wpilib.system.Notifier;
import org.wpilib.units.AngularAccelerationUnit;
import org.wpilib.units.measure.Angle;
import org.wpilib.units.measure.AngularAcceleration;
import org.wpilib.units.measure.AngularVelocity;
import org.wpilib.units.measure.Current;
import org.wpilib.units.measure.Distance;
import org.wpilib.units.measure.LinearAcceleration;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.units.measure.Temperature;
import org.wpilib.units.measure.Time;
import org.wpilib.units.measure.Velocity;
import org.wpilib.units.measure.Voltage;

/**
 * Smart motor controller wrapper for motor controllers.
 */
public abstract class SmartMotorController
{
  /**
   * {@link SmartMotorControllerConfig} for the motor.
   */
  protected SmartMotorControllerConfig m_config;
  /**
   * {@link ClosedLoopControllerSlot} for the closed loop controller.
   */
  protected ClosedLoopControllerSlot   m_slot                       = ClosedLoopControllerSlot.SLOT_0;
  /**
   * Exponential profile for the closed loop controller.
   */
  protected Optional<ExponentialProfile>       m_expoProfile = Optional.empty();
  /**
   * Exponential profile state for the closed loop controller.
   */
  protected Optional<ExponentialProfile.State> m_expoState        = Optional.empty();
  /**
   * Trapezoidal profile for the closed loop controller.
   */
  protected Optional<TrapezoidProfile>         m_trapezoidProfile = Optional.empty();
  /**
   * Trapezoidal profile state for the closed loop controller.
   */
  protected Optional<TrapezoidProfile.State> m_trapState = Optional.empty();
  /**
   * Simple PID controller for the motor controller.
   */
  protected Optional<PIDController>    m_pid            = Optional.empty();
  /**
   * Setpoint position
   */
  protected Optional<Angle>            setpointPosition             = Optional.empty();
  /**
   * Setpoint velocity.
   */
  protected Optional<AngularVelocity> setpointVelocity             = Optional.empty();
  /**
   * Thread of the closed loop controller.
   */
  protected Notifier               m_closedLoopControllerThread = null;
  /**
   * Loosely coupled followers.
   */
  protected Optional<SmartMotorController[]>              m_looseFollowers              = Optional.empty();
  /**
   * Running status of the closed loop controller.
   */
  private   boolean                                       m_closedLoopControllerRunning = false;

  /**
   * Create a {@link SmartMotorController} wrapper from the provided motor controller object.
   *
   * @param motorController Motor controller object.
   * @param motorSim        {@link DCMotor} which the motor controller is connected too.
   * @param cfg             {@link SmartMotorControllerConfig} for the {@link SmartMotorController}
   * @return {@link SmartMotorController}.
   */
  public static SmartMotorController create(Object motorController, DCMotor motorSim, SmartMotorControllerConfig cfg)
  {
    return null;
  }

  /**
   * Get the current encoder trapezoidal profile state for the closed loop controller.
   *
   * @return {@link TrapezoidProfile.State} from the encoders.
   */
  protected Optional<TrapezoidProfile.State> getTrapezoidalProfileState()
  {
    if (m_trapezoidProfile.isEmpty())
    {return Optional.empty();}
    if (m_config.getLinearClosedLoopControllerUse())
    {return Optional.of(new State(getMeasurementPosition().in(Meters), getMeasurementVelocity().in(MetersPerSecond)));}
    return Optional.of(new State(getMechanismPosition().in(Rotations), getMechanismVelocity().in(RotationsPerSecond)));
  }

  /**
   * Get the current encoder exponential profile state for the closed loop controller.
   *
   * @return {@link ExponentialProfile.State} from the encoders.
   */
  protected Optional<ExponentialProfile.State> getExponentialProfileState()
  {
    if (m_expoProfile.isEmpty())
    {return Optional.empty();}
    if (m_config.getLinearClosedLoopControllerUse())
    {
      return Optional.of(new ExponentialProfile.State(getMeasurementPosition().in(Meters),
                                                      getMeasurementVelocity().in(MetersPerSecond)));
    }
    return Optional.of(new ExponentialProfile.State(getMechanismPosition().in(Rotations),
                                                    getMechanismVelocity().in(RotationsPerSecond)));
  }

  /**
   * Stop the closed loop controller.
   *
   */
  public void stopClosedLoopController()
  {
    if (m_closedLoopControllerThread != null)
    {
      m_closedLoopControllerThread.stop();
      m_closedLoopControllerRunning = false;
    }
  }

  /**
   * Start the closed loop controller with the period.
   *
   */
  public void startClosedLoopController()
  {
    if (m_closedLoopControllerThread != null && m_config.getMotorControllerMode() == ControlMode.CLOSED_LOOP)
    {
      m_pid.ifPresent(PIDController::reset);
      m_trapState = getTrapezoidalProfileState();
      m_expoState = getExponentialProfileState();
      m_closedLoopControllerThread.stop();
      m_closedLoopControllerThread.startPeriodic(m_config.getClosedLoopControlPeriod().orElse(Milliseconds.of(20))
                                                         .in(Seconds));
      m_closedLoopControllerRunning = true;
    }
  }

  /**
   * Iterate the closed loop controller. Feedforward are only applied with profiled pid controllers.
   */
  public void iterateClosedLoopController()
  {
    AtomicReference<Boolean> velocityTrapezoidalProfile = new AtomicReference<>(false);
    AtomicReference<ExponentialProfile.State> nextExpoState =
        new AtomicReference<>(new ExponentialProfile.State(0.0, 0.0));
    AtomicReference<TrapezoidProfile.State> nextTrapState =
        new AtomicReference<>(new TrapezoidProfile.State(0.0, 0.0));
    AtomicReference<Double>          pidOutputVoltage       = new AtomicReference<>((double) 0);
    AtomicReference<Double>          feedforward            = new AtomicReference<>(0.0);
    Optional<Angle>                  mechLowerLimit         = m_config.getMechanismLowerLimit();
    Optional<Angle>               mechUpperLimit      = m_config.getMechanismUpperLimit();
    Optional<ArmFeedforward>      armFeedforward      = m_config.getArmFeedforward(m_slot);
    Optional<ElevatorFeedforward>    elevatorFeedforward    = m_config.getElevatorFeedforward(m_slot);
    Optional<SimpleMotorFeedforward> simpleMotorFeedforward = m_config.getSimpleFeedforward(m_slot);
    Optional<Temperature> temperatureCutoff = m_config.getTemperatureCutoff();
    Optional<Voltage>     maximumVoltage    = m_config.getClosedLoopControllerMaximumVoltage();
    synchronizeRelativeEncoder();

    if (!m_closedLoopControllerRunning)
    {return;}

    if (setpointPosition.isPresent())
    {
      if (mechLowerLimit.isPresent())
      {
        if (setpointPosition.get().lt(mechLowerLimit.get()))
        {
          // TODO: THrow warning
          setpointPosition = mechLowerLimit;
        }
      }
      if (mechUpperLimit.isPresent())
      {
        if (setpointPosition.get().gt(mechUpperLimit.get()))
        {
          // TODO: THrow warning
          setpointPosition = mechUpperLimit;
        }
      }
    }

    // Get the motion profile setpoints
    if (setpointPosition.isPresent())
    {
      var setpoint = setpointPosition.get().in(Rotations);
      var position = getMechanismPosition().in(Rotations);
      var velocity = getMechanismVelocity().in(RotationsPerSecond);
      var loopTime = m_config.getClosedLoopControlPeriod()
                             .orElse(Milliseconds.of(20)).in(Seconds);

      // Change position and velocity to Meters and Meters per Second
      if (m_config.getLinearClosedLoopControllerUse())
      {
        position = getMeasurementPosition().in(Meters);
        velocity = getMeasurementVelocity().in(MetersPerSecond);
        setpoint = m_config.convertFromMechanism(setpointPosition.orElseThrow()).in(Meters);
      }

      if (m_expoProfile.isPresent())
      {
        nextExpoState.set(m_expoProfile.get().calculate(loopTime,
                                                        m_expoState
                                                            .orElse(new ExponentialProfile.State(position, velocity)),
                                                        new ExponentialProfile.State(setpoint, 0)));
      } else if (m_trapezoidProfile.isPresent())
      {
        nextTrapState.set(m_trapezoidProfile.get().calculate(loopTime,
                                                             m_trapState
                                                                 .orElse(new TrapezoidProfile.State(position,
                                                                                                    velocity)),
                                                             new TrapezoidProfile.State(setpoint, 0)));
      }
    } else if (setpointVelocity.isPresent())
    {
      var setpoint = setpointVelocity.get().in(RotationsPerSecond);
      var velocity = getMechanismVelocity().in(RotationsPerSecond);
      var loopTime = m_config.getClosedLoopControlPeriod()
                             .orElse(Milliseconds.of(20)).in(Seconds);

      // Change position and velocity to Meters and Meters per Second
      if (m_config.getLinearClosedLoopControllerUse())
      {
        velocity = getMeasurementVelocity().in(MetersPerSecond);
        setpoint = m_config.convertFromMechanism(setpointVelocity.orElseThrow()).in(MetersPerSecond);
      }

      if (m_trapezoidProfile.isPresent())
      {
        // TODO: 2027, Derive acceleration from SMCs
        nextTrapState.set(m_trapezoidProfile.get().calculate(loopTime,
                                                             m_trapState
                                                                 .orElse(new TrapezoidProfile.State(velocity, 0)),
                                                             new TrapezoidProfile.State(setpoint, 0)));
        velocityTrapezoidalProfile.set(true);
      }
    }

    // Get the PID output
    if (setpointPosition.isPresent())
    {
      var measured        = getMechanismPosition().in(Rotations);
      var setpoint        = setpointPosition.get().in(Rotations);
      var velocityProfile = 0.0;

      // Set the measured value and setpoint to Meters, if linear
      if (m_config.getLinearClosedLoopControllerUse())
      {
        measured = getMeasurementPosition().in(Meters);
        setpoint = m_config.convertFromMechanism(setpointPosition.get()).in(Meters); // Convert setpoint to Meters
      }

      if (m_expoProfile.isPresent())
      {
        setpoint = nextExpoState.get().position; // Rotations or Meters; depending on config
        velocityProfile = nextExpoState.get().velocity; // RotationsPerSecond or MetersPerSecond; depending on config
      } else if (m_trapezoidProfile.isPresent() && !m_config.getVelocityTrapezoidalProfileInUse())
      {
        setpoint = nextTrapState.get().position; // Rotations or Meters; depending on config
        velocityProfile = nextTrapState.get().velocity; // RotationsPerSecond or MetersPerSecond; depending on config
      }

      // Set the controller
      double finalMeasured        = measured;
      double finalSetpoint        = setpoint;
      double finalVelocityProfile = velocityProfile;
      m_pid.ifPresent(pidController -> pidOutputVoltage.set(pidController.calculate(finalMeasured, finalSetpoint)));

    } else if (setpointVelocity.isPresent())
    {
      var setpoint = setpointVelocity.get().in(RotationsPerSecond);
      var velocity = getMechanismVelocity().in(RotationsPerSecond);

      // Set the measured value and setpoint to Meters, if linear
      if (m_config.getLinearClosedLoopControllerUse())
      {
        velocity = getMeasurementVelocity().in(MetersPerSecond);
        setpoint = m_config.convertFromMechanism(setpointVelocity.get())
                           .in(MetersPerSecond); // Convert setpoint to Meters
      }

      if (m_trapezoidProfile.isPresent() && m_config.getVelocityTrapezoidalProfileInUse())
      {
        setpoint = nextTrapState.get().position; // Poorly named, in a velocity control loop, this is the setpoint velocity.
        var acceleration = nextTrapState.get().velocity; // Again poorly named, this is the setpoint acceleration.
      }

      double finalVelocity = velocity;
      double finalSetpoint = setpoint;
      m_pid.ifPresent(pidController -> pidOutputVoltage.set(pidController.calculate(finalVelocity, finalSetpoint)));
    }

    armFeedforward.ifPresent(ff -> {
      var profiled = (m_expoProfile.isPresent() || m_trapezoidProfile.isPresent());
      if (profiled && !velocityTrapezoidalProfile.get())
      {
        var currentVelocitySetpoint = RotationsPerSecond.of(
            m_trapState.isPresent() ? m_trapState.get().velocity
                                    : (m_expoState.isPresent() ? m_expoState.get().velocity : 0.0));
        var nextVelocitySetpoint = RotationsPerSecond.of(
            m_trapezoidProfile.isPresent() ? nextTrapState.get().velocity
                                           : (m_expoProfile.isPresent() ? nextExpoState.get().velocity : 0.0));
        feedforward.set(ff.calculate(getMechanismPosition().in(Radians),
                                                   currentVelocitySetpoint.in(RadiansPerSecond),
                                                   nextVelocitySetpoint.in(RadiansPerSecond)));
      } else
      {
        // When using a velocity profile the next velocity is the "position" (poorly named)
        var nextVelocitySetpoint = velocityTrapezoidalProfile.get() ? nextTrapState.get().position
                                                                    : setpointVelocity.orElse(RotationsPerSecond.zero())
                                                                                      .in(RotationsPerSecond);
        // Not profiled, so using current velocity or setpoint velocity.
        ff.calculate(getMechanismPosition().in(Radians),
                                   getMechanismVelocity().in(RadiansPerSecond),
                                   nextVelocitySetpoint);
      }
    });

    elevatorFeedforward.ifPresent(ff -> {
      var profiled = (m_expoProfile.isPresent() || m_trapezoidProfile.isPresent()) && setpointPosition.isPresent();
      if (profiled && !velocityTrapezoidalProfile.get())
      {
        var currentVelocitySetpoint = MetersPerSecond.of(
            m_trapState.isPresent() ? m_trapState.get().velocity
                                    : (m_expoState.isPresent() ? m_expoState.get().velocity : 0.0));
        var nextVelocitySetpoint = MetersPerSecond.of(
            m_trapezoidProfile.isPresent() ? nextTrapState.get().velocity
                                           : (m_expoProfile.isPresent() ? nextExpoState.get().velocity : 0.0));

        feedforward.set(ff.calculate(currentVelocitySetpoint.in(MetersPerSecond),
                                                   nextVelocitySetpoint.in(MetersPerSecond)));
      } else
      {
        // TODO: Implement velocity profile
        // Not profiled, so using current velocity or setpoint velocity.
        feedforward.set(ff.calculate(getMeasurementVelocity().in(MetersPerSecond), 0));
      }
    });

    simpleMotorFeedforward.ifPresent(ff -> {
      var profiled = (m_expoProfile.isPresent() || m_trapezoidProfile.isPresent());
      if (profiled && !velocityTrapezoidalProfile.get())
      {
        var currentVelocitySetpoint = RotationsPerSecond.of(
            m_trapState.isPresent() ? m_trapState.get().velocity
                                    : (m_expoState.isPresent() ? m_expoState.get().velocity : 0.0));
        var nextVelocitySetpoint = RotationsPerSecond.of(
            m_trapezoidProfile.isPresent() ? nextTrapState.get().velocity
                                           : (m_expoProfile.isPresent() ? nextExpoState.get().velocity : 0.0));
        feedforward.set(ff.calculate(currentVelocitySetpoint.in(RotationsPerSecond),
                                                   nextVelocitySetpoint.in(RotationsPerSecond)));

      } else
      {
        // When using a velocity profile the next velocity is the "position" (poorly named)
        var nextVelocitySetpoint = velocityTrapezoidalProfile.get() ? nextTrapState.get().position
                                                                    : setpointVelocity.orElse(RotationsPerSecond.zero())
                                                                                      .in(RotationsPerSecond);
        // Not profiled, so using current velocity, or setpoint velocity.
        feedforward.set(ff.calculate(getMechanismVelocity().in(RotationsPerSecond),
                                                   nextVelocitySetpoint));
      }
    });

    // Set the current states in the class.
    if (m_expoProfile.isPresent())
    {m_expoState = Optional.of(nextExpoState.get());}
    if (m_trapezoidProfile.isPresent())
    {m_trapState = Optional.of(nextTrapState.get());}

    // Boundary check.
    if (mechUpperLimit.isPresent())
    {
      if (getMechanismPosition().gt(mechUpperLimit.get()) &&
          (pidOutputVoltage.get() + feedforward.get()) > 0)
      {
        feedforward.set(0.0);
        pidOutputVoltage.set(0.0);
      }
    }
    if (mechLowerLimit.isPresent())
    {
      if (getMechanismPosition().lt(mechLowerLimit.get()) &&
          (pidOutputVoltage.get() + feedforward.get()) < 0)
      {
        feedforward.set(0.0);
        pidOutputVoltage.set(0.0);
      }
    }
    if (temperatureCutoff.isPresent())
    {
      if (getTemperature().gte(temperatureCutoff.get()))
      {
        feedforward.set(0.0);
        pidOutputVoltage.set(0.0);
      }
    }
    double outputVoltage = pidOutputVoltage.get() + feedforward.get();
    if (maximumVoltage.isPresent())
    {
      double maxVolts = maximumVoltage.get().in(Volts);
      outputVoltage = Volts.of(outputVoltage).gt(maximumVoltage.get()) ? maxVolts : Volts.of(outputVoltage).lt(maximumVoltage.get().times(-1)) ? -maxVolts : outputVoltage;
    }
    setVoltage(Volts.of(outputVoltage));
  }

  /**
   * Setup the simulation for the wrapper.
   */
  public abstract void setupSimulation();

  /**
   * Seed the relative encoder with the position from the absolute encoder.
   */
  public abstract void seedRelativeEncoder();

  /**
   * Check if the relative encoder is out of sync with absolute encoder within defined tolerances.
   */
  public abstract void synchronizeRelativeEncoder();

  /**
   * Simulation iteration.
   */
  public abstract void simIterate();

  /**
   * Set the motor idle mode from COAST or BRAKE.
   *
   * @param mode {@link IdleMode} selected.
   */
  public abstract void setIdleMode(IdleMode mode);

  /**
   * Set the encoder velocity
   *
   * @param velocity {@link AngularVelocity} of the Mechanism.
   */
  public abstract void setEncoderVelocity(AngularVelocity velocity);

  /**
   * Set the encoder velocity.
   *
   * @param velocity Measurement {@link LinearVelocity}
   */
  public abstract void setEncoderVelocity(LinearVelocity velocity);

  /**
   * Set the encoder position
   *
   * @param angle Current Mechanism {@link Angle}.
   */
  public abstract void setEncoderPosition(Angle angle);

  /**
   * Set the encoder position.
   *
   * @param distance Current Measurement {@link Distance}.
   */
  public abstract void setEncoderPosition(Distance distance);

  /**
   * Set the Mechanism {@link Angle} using the PID and feedforward from {@link SmartMotorControllerConfig}.
   *
   * @param angle Mechanism angle to set.
   */
  public abstract void setPosition(Angle angle);

  /**
   * Set the Mechanism {@link Distance} using the PID and feedforward from {@link SmartMotorControllerConfig}.
   *
   * @param distance Mechanism {@link Distance} to set.
   */
  public abstract void setPosition(Distance distance);

  /**
   * Set the Mechanism {@link LinearVelocity} using the PID and feedforward from {@link SmartMotorControllerConfig}.
   *
   * @param velocity Mechanism {@link LinearVelocity} to target.
   */
  public abstract void setVelocity(LinearVelocity velocity);

  /**
   * Set the Mechanism {@link AngularVelocity} using the PID and feedforward from {@link SmartMotorControllerConfig}.
   *
   * @param angle Mechanism {@link AngularVelocity} to target.
   */
  public abstract void setVelocity(AngularVelocity angle);

  /**
   * Apply the {@link SmartMotorControllerConfig} to the {@link SmartMotorController}.
   *
   * @param config {@link SmartMotorControllerConfig} to use.
   * @return Successful Application of the configuration.
   */
  public abstract boolean applyConfig(SmartMotorControllerConfig config);

  /**
   * Get the duty cycle output of the motor controller.
   *
   * @return DutyCyle of the motor controller.
   */
  public abstract double getDutyCycle();

  /**
   * Set the dutycycle output of the motor controller.
   *
   * @param dutyCycle Value between [-1,1]
   */
  public abstract void setDutyCycle(double dutyCycle);

  /**
   * Get the supply current of the motor controller.
   *
   * @return The supply current of the motor controller.
   */
  public abstract Optional<Current> getSupplyCurrent();

  /**
   * Get the stator current of the motor controller.
   *
   * @return Stator current
   */
  public abstract Current getStatorCurrent();

  /**
   * Get the voltage output of the motor.
   *
   * @return Voltage output of the motor.
   */
  public abstract Voltage getVoltage();

  /**
   * Set the voltage output of the motor controller. Useful for Sysid.
   *
   * @param voltage Voltage to set the motor controller output to.
   */
  public abstract void setVoltage(Voltage voltage);

  /**
   * Get the {@link DCMotor} modeling the motor controlled by the motor controller.
   *
   * @return {@link DCMotor} of the controlled motor.
   */
  public abstract DCMotor getDCMotor();


  /**
   * Get the usable measurement of the motor for mechanisms operating under distance units converted with the
   * {@link SmartMotorControllerConfig}.
   *
   * @return Measurement velocity of the mechanism post-gearing.
   */
  public abstract LinearVelocity getMeasurementVelocity();

  /**
   * Get the usable measurement of the motor for mechanisms operating under distance units converted with the
   * {@link SmartMotorControllerConfig}.
   *
   * @return Measurement velocity of the mechanism post-gearing.
   */
  public abstract Distance getMeasurementPosition();

  /**
   * Get the usable measurement of the motor for mechanisms operating under distance units converted with the
   * {@link SmartMotorControllerConfig}
   *
   * @return Measurement acceleration of the mechanism post-gearing.
   */
  public abstract LinearAcceleration getMeasurementAcceleration();

  /**
   * Get the Mechanism {@link AngularVelocity} taking the configured reductionGearing into the measurement
   * applied via the {@link SmartMotorControllerConfig}.
   *
   * @return Mechanism {@link AngularVelocity}
   */
  public abstract AngularVelocity getMechanismVelocity();

  /**
   * Get the Mechanism {@link AngularAcceleration}, calculating it on the robot controller if necessary by taking the
   * derivative of the velocity.
   *
   * @return Mechanism {@link AngularAcceleration}
   */
  public abstract AngularAcceleration getMechanismAcceleration();

  /**
   * Get the mechanism {@link Angle} taking the configured reductionGearing from
   * {@link SmartMotorControllerConfig}.
   *
   * @return Mechanism {@link Angle}
   */
  public abstract Angle getMechanismPosition();

  /**
   * Gets the angular velocity of the motor.
   *
   * @return {@link AngularVelocity} of the relative motor encoder.
   */
  public abstract AngularVelocity getRotorVelocity();

  /**
   * Get the rotations of the motor with the relative encoder since the motor controller powered on scaled to the
   * mechanism rotations.
   *
   * @return {@link Angle} of the relative encoder in the motor.
   */
  public abstract Angle getRotorPosition();

  /**
   * Get the rotations of the mechanism according to the external encoder.
   *
   * @return {@link Angle} of the external encoder in the mechanism.
   */
  public abstract Optional<Angle> getExternalEncoderPosition();

  /**
   * Get the velocity of the mechanism according to the external encoder.
   *
   * @return {@link AngularVelocity} of the external encoder in the mechanism.
   */
  public abstract Optional<AngularVelocity> getExternalEncoderVelocity();

  /**
   * Set the inversion state of the motor.
   *
   * @param inverted Inverted motor.
   */
  public abstract void setMotorInverted(boolean inverted);

  /**
   * Set the phase of the encoder attached to the brushless motor.
   *
   * @param inverted Phase of the encoder.
   */
  public abstract void setEncoderInverted(boolean inverted);

  /**
   * Set the maximum velocity of the trapezoidal profile for the feedback controller.
   *
   * @param maxVelocity Maximum velocity, will be translated to MetersPerSecond.
   */
  public abstract void setMotionProfileMaxVelocity(LinearVelocity maxVelocity);

  /**
   * Set the maximum acceleration of the trapezoidal profile for the feedback controller.
   *
   * @param maxAcceleration Maximum acceleration, will be translated to MetersPerSecondPerSecond.
   */
  public abstract void setMotionProfileMaxAcceleration(LinearAcceleration maxAcceleration);

  /**
   * Set the maximum velocity for the trapezoidal profile for the feedback controller.
   *
   * @param maxVelocity Maximum velocity, will be translated to RotationsPerSecond.
   */
  public abstract void setMotionProfileMaxVelocity(AngularVelocity maxVelocity);

  /**
   * Set the maximum acceleration for the trapezoidal profile for the feedback controller.
   *
   * @param maxAcceleration Maximum acceleration, will be translated to RotationsPerSecondPerSecond.
   */
  public abstract void setMotionProfileMaxAcceleration(AngularAcceleration maxAcceleration);

  /**
   * Set the maximum jerk for the trapezoidal profile for the feedback controller.
   *
   * @param maxJerk Maximum jerk, will be translated to RotationsPerSecondPerSecondPerSecond.
   */
  public abstract void setMotionProfileMaxJerk(Velocity<AngularAccelerationUnit> maxJerk);

  /**
   * Set the exponential profile fields.
   *
   * @param kV       kV for the exponential profile.
   * @param kA       kA for the exponential profile.
   * @param maxInput Maximum input for the exponential profile.
   */
  public abstract void setExponentialProfile(OptionalDouble kV, OptionalDouble kA, Optional<Voltage> maxInput);

  /**
   * Set kP for the feedback controller PID.
   *
   * @param kP kP
   */
  public abstract void setKp(double kP);

  /**
   * Set kI for the feedback controller PID.
   *
   * @param kI kI.
   */
  public abstract void setKi(double kI);

  /**
   * Set kD for the feedback controller PID.
   *
   * @param kD kD for the feedback controller PID.
   */
  public abstract void setKd(double kD);

  /**
   * Set the closed loop feedback controller PID.
   *
   * @param kP kP; Proportional scalar.
   * @param kI kI; Integral scalar.
   * @param kD kD; derivative scalar.
   */
  public abstract void setFeedback(double kP, double kI, double kD);

  /**
   * Static feedforward element.
   *
   * @param kS kS; Static feedforward.
   */
  public abstract void setKs(double kS);

  /**
   * Velocity feedforward element.
   *
   * @param kV kV; Velocity feedforward.
   */
  public abstract void setKv(double kV);

  /**
   * Acceleration feedforward element.
   *
   * @param kA kA; Acceleration feedforward.
   */
  public abstract void setKa(double kA);

  /**
   * kSin feedforward element.
   *
   * @param kG kG; Gravity feedforward.
   */
  public abstract void setKg(double kG);

  /**
   * Set the feedforward controller.
   *
   * @param kS kS; Static feedforward.
   * @param kV kV; Velocity feedforward.
   * @param kA kA; Acceleration feedforward.
   * @param kG kG; Gravity feedforward.
   */
  public abstract void setFeedforward(double kS, double kV, double kA, double kG);

  /**
   * Set the stator current limit for the device.
   *
   * @param currentLimit Stator current limit.
   */
  public abstract void setStatorCurrentLimit(Current currentLimit);

  /**
   * Set the supply current limit.
   *
   * @param currentLimit Supply current limit.
   */
  public abstract void setSupplyCurrentLimit(Current currentLimit);

  /**
   * Set the closed loop ramp rate. The ramp rate is how fast the motor can go from 0-100, measured in seconds.
   *
   * @param rampRate Time from 0 to 100.
   */
  public abstract void setClosedLoopRampRate(Time rampRate);

  /**
   * Set the open loop ramp rate. The ramp rate is how fast the motor can go from 0 to 100, measured in Seconds.
   *
   * @param rampRate Time it takes to go from 0 to 100.
   */
  public abstract void setOpenLoopRampRate(Time rampRate);

  /**
   * Set the measurement upper limit, only works if mechanism circumference is defined.
   *
   * @param upperLimit Upper limit, will be translated to meters.
   */
  public abstract void setMeasurementUpperLimit(Distance upperLimit);

  /**
   * Set the measurement lower limit, only works if mechanism circumference is defined.
   *
   * @param lowerLimit Lower limit, will be translated to meters.
   */
  public abstract void setMeasurementLowerLimit(Distance lowerLimit);

  /**
   * Set the mechanism upper limit.
   *
   * @param upperLimit Upper limit, will be translated to rotations.
   */
  public abstract void setMechanismUpperLimit(Angle upperLimit);

  /**
   * Set the mechanism lower limit.
   *
   * @param lowerLimit Lower limit, will be translated to rotations.
   */
  public abstract void setMechanismLowerLimit(Angle lowerLimit);

  /**
   * Set the Mechanism limits for the motor controller.
   *
   * @param lower Lower limit, will be translated to rotations.
   * @param upper Upper limit, will be translated to rotations.
   */
  public abstract void setMechanismLimits(Angle lower, Angle upper);

  /**
   * Enable or disable the mechanism/measurement limits in the motor controller.
   *
   * @param enabled Application of the limits
   */
  public abstract void setMechanismLimitsEnabled(boolean enabled);

  /**
   * Set the closed loop controller slot to use.
   *
   * @param slot Slot to use.
   */
  public abstract void setClosedLoopSlot(ClosedLoopControllerSlot slot);

  /**
   * Get the {@link SmartMotorController} temperature.
   *
   * @return {@link Temperature}
   */
  public abstract Temperature getTemperature();

  /**
   * Get the {@link SmartMotorControllerConfig} for the {@link SmartMotorController}
   *
   * @return {@link SmartMotorControllerConfig} used.
   */
  public abstract SmartMotorControllerConfig getConfig();

  /**
   * Get the Motor Controller Object passed into the {@link SmartMotorController}.
   *
   * @return Motor Controller object.
   */
  public abstract Object getMotorController();

  /**
   * Get the motor controller object config generated by {@link SmartMotorController} based off the
   * {@link SmartMotorControllerConfig}
   *
   * @return Motor controller config.
   */
  public abstract Object getMotorControllerConfig();

  /**
   * Get the Mechanism setpoint position.
   *
   * @return Mechanism Setpoint position.
   */
  public Optional<Angle> getMechanismPositionSetpoint()
  {
    return setpointPosition;
  }

  /**
   * Get the Mechanism velocity setpoint.
   *
   * @return Mechanism velocity setpoint.
   */
  public Optional<AngularVelocity> getMechanismSetpointVelocity()
  {
    return setpointVelocity;
  }

  /**
   * Get the name of the {@link SmartMotorController}
   *
   * @return {@link String} name if present, else "SmartMotorController"
   */
  public String getName()
  {
    return m_config.getTelemetryName().orElse("SmartMotorController");
  }

  /**
   * Close the SMC for unit testing.
   */
  public void close()
  {
    if (m_closedLoopControllerThread != null)
    {
      m_closedLoopControllerThread.stop();
      m_closedLoopControllerThread.close();
      m_closedLoopControllerThread = null;
    }
  }

  @Override
  public String toString()
  {
    return getName();
  }

  /**
   * Get the active closed loop controller slot.
   *
   * @return Active closed loop controller slot.
   */
  public ClosedLoopControllerSlot getClosedLoopControllerSlot()
  {
    return m_slot;
  }

  /**
   * Current closed loop controller slot.
   */
  public enum ClosedLoopControllerSlot
  {
    /**
     * Slot 0 is the default slot for the closed loop controller.
     */
    SLOT_0,
    /**
     * Slot 1 is the second slot for the closed loop controller.
     */
    SLOT_1,
    /**
     * Slot 2 is the third slot for the closed loop controller.
     */
    SLOT_2,
    /**
     * Slot 3 is the fourth slot for the closed loop controller.
     */
    SLOT_3
  }
}
