// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.sysid;

import static edu.wpi.first.units.Units.Second;
import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.VoltageUnit;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.units.measure.Velocity;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.sysid.SysIdRoutineLog;
import java.util.function.Consumer;
import org.wpilib.commands3.Command;
import org.wpilib.commands3.Mechanism;

/**
 * A SysId characterization routine for a single mechanism. Mechanisms may have multiple motors.
 *
 * <p>A single subsystem may have multiple mechanisms, but mechanisms should not share test
 * routines. Each complete test of a mechanism should have its own SysIdRoutine instance, since the
 * log name of the recorded data is determined by the mechanism name.
 *
 * <p>The test state (e.g. "quasistatic-forward") is logged once per iteration during test
 * execution, and once with state "none" when a test ends. Motor frames are logged every iteration
 * during test execution.
 *
 * <p>Timestamps are not coordinated across data, so motor frames and test state tags may be
 * recorded on different log frames. Because frame alignment is not guaranteed, SysId parses the log
 * by using the test state flag to determine the timestamp range for each section of the test, and
 * then extracts the motor frames within the valid timestamp ranges. If a given test was run
 * multiple times in a single logfile, the user will need to select which of the tests to use for
 * the fit in the analysis tool.
 */
public class SysIdRoutine extends SysIdRoutineLog {
  private final Config m_config;
  private final SysIdMechanism m_mechanism;
  private final Consumer<State> m_recordState;

  /**
   * Create a new SysId characterization routine.
   *
   * @param config Hardware-independent parameters for the SysId routine.
   * @param mechanism Hardware interface for the SysId routine.
   */
  public SysIdRoutine(Config config, SysIdMechanism mechanism) {
    super(mechanism.m_name);
    m_config = config;
    m_mechanism = mechanism;
    m_recordState = config.m_recordState != null ? config.m_recordState : this::recordState;
  }

  /** Hardware-independent configuration for a SysId test routine. */
  public static class Config {
    /** The voltage ramp rate used for quasistatic test routines. */
    public final Velocity<VoltageUnit> m_rampRate;

    /** The step voltage output used for dynamic test routines. */
    public final Voltage m_stepVoltage;

    /** Safety timeout for the test routine commands. */
    public final Time m_timeout;

    /** Optional handle for recording test state in a third-party logging solution. */
    public final Consumer<State> m_recordState;

    /**
     * Create a new configuration for a SysId test routine.
     *
     * @param rampRate The voltage ramp rate used for quasistatic test routines. Defaults to 1 volt
     *     per second if left null.
     * @param stepVoltage The step voltage output used for dynamic test routines. Defaults to 7
     *     volts if left null.
     * @param timeout Safety timeout for the test routine commands. Defaults to 10 seconds if left
     *     null.
     * @param recordState Optional handle for recording test state in a third-party logging
     *     solution. If provided, the test routine state will be passed to this callback instead of
     *     logged in WPILog.
     */
    public Config(
        Velocity<VoltageUnit> rampRate,
        Voltage stepVoltage,
        Time timeout,
        Consumer<State> recordState) {
      m_rampRate = rampRate != null ? rampRate : Volts.of(1).per(Second);
      m_stepVoltage = stepVoltage != null ? stepVoltage : Volts.of(7);
      m_timeout = timeout != null ? timeout : Seconds.of(10);
      m_recordState = recordState;
    }

    /**
     * Create a new configuration for a SysId test routine.
     *
     * @param rampRate The voltage ramp rate used for quasistatic test routines. Defaults to 1 volt
     *     per second if left null.
     * @param stepVoltage The step voltage output used for dynamic test routines. Defaults to 7
     *     volts if left null.
     * @param timeout Safety timeout for the test routine commands. Defaults to 10 seconds if left
     *     null.
     */
    public Config(Velocity<VoltageUnit> rampRate, Voltage stepVoltage, Time timeout) {
      this(rampRate, stepVoltage, timeout, null);
    }

    /**
     * Create a default configuration for a SysId test routine with all default settings.
     *
     * <p>rampRate: 1 volt/sec
     *
     * <p>stepVoltage: 7 volts
     *
     * <p>timeout: 10 seconds
     */
    public Config() {
      this(null, null, null, null);
    }
  }

  /**
   * A mechanism to be characterized by a SysId routine. Defines callbacks needed for the SysId test
   * routine to control and record data from the mechanism.
   */
  public static class SysIdMechanism {
    /** Sends the SysId-specified drive signal to the mechanism motors during test routines. */
    public final Consumer<? super Voltage> m_drive;

    /**
     * Returns measured data (voltages, positions, velocities) of the mechanism motors during test
     * routines.
     */
    public final Consumer<SysIdRoutineLog> m_log;

    /** The subsystem containing the motor(s) that is (or are) being characterized. */
    public final Mechanism m_subsystem;

    /** The name of the mechanism being tested. */
    public final String m_name;

    /**
     * Create a new mechanism specification for a SysId routine.
     *
     * @param drive Sends the SysId-specified drive signal to the mechanism motors during test
     *     routines.
     * @param log Returns measured data of the mechanism motors during test routines. To return
     *     data, call `motor(string motorName)` on the supplied `SysIdRoutineLog` instance, and then
     *     call one or more of the chainable logging handles (e.g. `voltage`) on the returned
     *     `MotorLog`. Multiple motors can be logged in a single callback by calling `motor`
     *     multiple times.
     * @param subsystem The subsystem containing the motor(s) that is (or are) being characterized.
     *     Will be declared as a requirement for the returned test commands.
     * @param name The name of the mechanism being tested. Will be appended to the log entry title
     *     for the routine's test state, e.g. "sysid-test-state-mechanism". Defaults to the name of
     *     the subsystem if left null.
     */
    public SysIdMechanism(
        Consumer<Voltage> drive, Consumer<SysIdRoutineLog> log, Mechanism subsystem, String name) {
      m_drive = drive;
      m_log = log != null ? log : l -> {};
      m_subsystem = subsystem;
      m_name = name != null ? name : subsystem.getName();
    }

    /**
     * Create a new mechanism specification for a SysId routine. Defaults the mechanism name to the
     * subsystem name.
     *
     * @param drive Sends the SysId-specified drive signal to the mechanism motors during test
     *     routines.
     * @param log Returns measured data of the mechanism motors during test routines. To return
     *     data, call `motor(string motorName)` on the supplied `SysIdRoutineLog` instance, and then
     *     call one or more of the chainable logging handles (e.g. `voltage`) on the returned
     *     `MotorLog`. Multiple motors can be logged in a single callback by calling `motor`
     *     multiple times.
     * @param subsystem The subsystem containing the motor(s) that is (or are) being characterized.
     *     Will be declared as a requirement for the returned test commands. The subsystem's `name`
     *     will be appended to the log entry title for the routine's test state, e.g.
     *     "sysid-test-state-subsystem".
     */
    public SysIdMechanism(
        Consumer<Voltage> drive, Consumer<SysIdRoutineLog> log, Mechanism subsystem) {
      this(drive, log, subsystem, null);
    }
  }

  /** Motor direction for a SysId test. */
  public enum Direction {
    /** Forward. */
    kForward,
    /** Reverse. */
    kReverse
  }

  /**
   * Returns a command to run a quasistatic test in the specified direction.
   *
   * <p>The command will call the `drive` and `log` callbacks supplied at routine construction once
   * per iteration. Upon command end or interruption, the `drive` callback is called with a value of
   * 0 volts.
   *
   * @param direction The direction in which to run the test.
   * @return A command to run the test.
   */
  public Command quasistatic(Direction direction) {
    double outputSign = switch (direction) {
      case kForward -> 1.0;
      case kReverse -> -1.0;
    };
    State state = switch (direction) {
      case kForward -> State.kDynamicForward;
      case kReverse -> State.kDynamicReverse;
    };

    return m_mechanism.m_subsystem.run(co -> {
      Timer timer = new Timer();
      timer.start();
      while (!timer.hasElapsed(m_config.m_timeout.in(Seconds))) {
        m_mechanism.m_drive.accept((Voltage)m_config.m_rampRate.times(Seconds.of(timer.get() * outputSign)));
        m_mechanism.m_log.accept(this);
        m_recordState.accept(state);
        co.yield();
      }
      m_mechanism.m_drive.accept(Volts.of(0));
      m_recordState.accept(State.kNone);
    }).whenCanceled(() -> {
      m_mechanism.m_drive.accept(Volts.of(0));
      m_recordState.accept(State.kNone);
    }).named("sysid-" + state.toString() + "-" + m_mechanism.m_name);
  }

  /**
   * Returns a command to run a dynamic test in the specified direction.
   *
   * <p>The command will call the `drive` and `log` callbacks supplied at routine construction once
   * per iteration. Upon command end or interruption, the `drive` callback is called with a value of
   * 0 volts.
   *
   * @param direction The direction in which to run the test.
   * @return A command to run the test.
   */
  public Command dynamic(Direction direction) {
    double outputSign = switch (direction) {
      case kForward -> 1.0;
      case kReverse -> -1.0;
    };
    State state = switch (direction) {
      case kForward -> State.kDynamicForward;
      case kReverse -> State.kDynamicReverse;
    };

    return m_mechanism.m_subsystem.run(co -> {
      Voltage output = m_config.m_stepVoltage.times(outputSign);
      Timer timer = new Timer();
      timer.start();

      while (!timer.hasElapsed(m_config.m_timeout.in(Seconds))) {
        m_mechanism.m_drive.accept(output);
        m_mechanism.m_log.accept(this);
        m_recordState.accept(state);
        co.yield();
      }
      m_mechanism.m_drive.accept(Volts.of(0));
      m_recordState.accept(State.kNone);
    }).whenCanceled(() -> {
      m_mechanism.m_drive.accept(Volts.of(0));
      m_recordState.accept(State.kNone);
    }).named("sysid-" + state + "-" + m_mechanism.m_name);

  }
}
