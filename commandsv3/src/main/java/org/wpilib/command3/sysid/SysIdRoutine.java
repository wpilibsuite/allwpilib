// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.sysid;

import static org.wpilib.units.Units.Second;
import static org.wpilib.units.Units.Seconds;
import static org.wpilib.units.Units.Volts;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.function.Consumer;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.sysid.SysIdRoutineLog;
import org.wpilib.system.Timer;
import org.wpilib.units.VoltageUnit;
import org.wpilib.units.measure.Time;
import org.wpilib.units.measure.Velocity;
import org.wpilib.units.measure.Voltage;

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
    super(mechanism.name);
    m_config = config;
    m_mechanism = mechanism;
    m_recordState = config.recordState != null ? config.recordState : this::recordState;
  }

  /** Hardware-independent configuration for a SysId test routine. */
  public static class Config {
    /** The voltage ramp rate used for quasistatic test routines. */
    public final Velocity<VoltageUnit> rampRate;

    /** The step voltage output used for dynamic test routines. */
    public final Voltage stepVoltage;

    /** Safety timeout for the test routine commands. */
    public final Time timeout;

    /** Optional handle for recording test state in a third-party logging solution. */
    public final Consumer<State> recordState;

    /** Default voltage ramp rate used in quasistatic test routines, set to 1 volt per second. */
    public static final Velocity<VoltageUnit> DEFAULT_RAMP_RATE = Volts.of(1).per(Second);

    /** Default step voltage used in dynamic test routines, set to 7 volts. */
    public static final Voltage DEFAULT_STEP_VOLTAGE = Volts.of(7);

    /** Default timeout used to automatically end test routine commands, set to 10 seconds. */
    public static final Time DEFAULT_TIMEOUT = Seconds.of(10);

    /**
     * Create a new configuration for a SysId test routine. To use the default config values, pass
     * in the provided static default fields.
     *
     * @param rampRate The voltage ramp rate used for quasistatic test routines.
     * @param stepVoltage The step voltage output used for dynamic test routines.
     * @param timeout Safety timeout for the test routine commands.
     * @param recordState Optional handle for recording test state in a third-party logging
     *     solution. If provided, the test routine state will be passed to this callback instead of
     *     logged in WPILog.
     */
    public Config(
        Velocity<VoltageUnit> rampRate,
        Voltage stepVoltage,
        Time timeout,
        Consumer<State> recordState) {
      requireNonNullParam(rampRate, "rampRate", "SysIdRoutine.Config");
      requireNonNullParam(stepVoltage, "stepVoltage", "SysIdRoutine.Config");
      requireNonNullParam(timeout, "timeout", "SysIdRoutine.Config");
      this.rampRate = rampRate;
      this.stepVoltage = stepVoltage;
      this.timeout = timeout;
      this.recordState = recordState;
    }

    /**
     * Create a new configuration for a SysId test routine.
     *
     * @param rampRate The voltage ramp rate used for quasistatic test routines.
     * @param stepVoltage The step voltage output used for dynamic test routines.
     * @param timeout Safety timeout for the test routine commands.
     * @return A Config object with the specified ramp rate, step voltage, and timeout that records
     *     to a DataLog file.
     */
    public static Config useDataLog(
        Velocity<VoltageUnit> rampRate, Voltage stepVoltage, Time timeout) {
      return new Config(rampRate, stepVoltage, timeout, null);
    }

    /**
     * Create a default configuration for a SysId test routine with all default settings.
     *
     * <p>rampRate: 1 volt/sec
     *
     * <p>stepVoltage: 7 volts
     *
     * <p>timeout: 10 seconds
     *
     * @return A Config object with the default ramp rate, step voltage, and timeout that records to
     *     a DataLog file.
     */
    public static Config defaults() {
      return new Config(DEFAULT_RAMP_RATE, DEFAULT_STEP_VOLTAGE, DEFAULT_TIMEOUT, null);
    }
  }

  /**
   * A mechanism to be characterized by a SysId routine. Defines callbacks needed for the SysId test
   * routine to control and record data from the mechanism.
   */
  public static class SysIdMechanism {
    /** Sends the SysId-specified drive signal to the mechanism motors during test routines. */
    public final Consumer<? super Voltage> drive;

    /**
     * Returns measured data (voltages, positions, velocities) of the mechanism motors during test
     * routines.
     */
    public final Consumer<SysIdRoutineLog> log;

    /** The mechanism containing the motor(s) that is (or are) being characterized. */
    public final Mechanism mechanism;

    /** The name of the mechanism being tested. */
    public final String name;

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
      this.drive = drive;
      this.log = log != null ? log : l -> {};
      mechanism = subsystem;
      this.name = name != null ? name : subsystem.getName();
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
    FORWARD,
    /** Reverse. */
    REVERSE
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
    double outputSign =
        switch (direction) {
          case FORWARD -> 1.0;
          case REVERSE -> -1.0;
        };
    State state =
        switch (direction) {
          case FORWARD -> State.QUASISTATIC_FORWARD;
          case REVERSE -> State.QUASISTATIC_REVERSE;
        };

    return m_mechanism
        .mechanism
        .run(
            co -> {
              Timer timer = Timer.createStarted();
              while (!timer.hasElapsed(m_config.timeout)) {
                m_mechanism.drive.accept(
                    (Voltage) m_config.rampRate.times(Seconds.of(timer.get() * outputSign)));
                m_mechanism.log.accept(this);
                m_recordState.accept(state);
                co.yield();
              }
              m_mechanism.drive.accept(Volts.of(0));
              m_recordState.accept(State.NONE);
            })
        .whenCanceled(
            () -> {
              m_mechanism.drive.accept(Volts.of(0));
              m_recordState.accept(State.NONE);
            })
        .named("sysid-" + state + "-" + m_mechanism.name);
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
    double outputSign =
        switch (direction) {
          case FORWARD -> 1.0;
          case REVERSE -> -1.0;
        };
    State state =
        switch (direction) {
          case FORWARD -> State.DYNAMIC_FORWARD;
          case REVERSE -> State.DYNAMIC_REVERSE;
        };

    return m_mechanism
        .mechanism
        .run(
            co -> {
              Voltage output = m_config.stepVoltage.times(outputSign);
              Timer timer = Timer.createStarted();
              while (!timer.hasElapsed(m_config.timeout)) {
                m_mechanism.drive.accept(output);
                m_mechanism.log.accept(this);
                m_recordState.accept(state);
                co.yield();
              }
              m_mechanism.drive.accept(Volts.of(0));
              m_recordState.accept(State.NONE);
            })
        .whenCanceled(
            () -> {
              m_mechanism.drive.accept(Volts.of(0));
              m_recordState.accept(State.NONE);
            })
        .named("sysid-" + state + "-" + m_mechanism.name);
  }
}
