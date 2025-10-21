package edu.wpi.first.wpilibj;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.DoubleSubscriber;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.PubSubOption;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Voltage;

/** This class controls a specific motor and encoder hooked up to an ExpansionHub. */
public class ExpansionHubMotor implements AutoCloseable {
  private static final int kPercentageMode = 0;
  private static final int kVoltageMode = 1;
  private static final int kPositionMode = 2;
  private static final int kVelocityMode = 3;

  private ExpansionHub m_hub;
  private final int m_channel;

  private final DoubleSubscriber m_encoderSubscriber;
  private final DoubleSubscriber m_encoderVelocitySubscriber;
  private final DoubleSubscriber m_currentSubscriber;

  private final DoublePublisher m_setpointPublisher;
  private final BooleanPublisher m_floatOn0Publisher;
  private final BooleanPublisher m_enabledPublisher;

  private final IntegerPublisher m_modePublisher;

  private final BooleanPublisher m_reversedPublisher;
  private final BooleanPublisher m_resetEncoderPublisher;

  private final DoublePublisher m_distancePerCountPublisher;

  private final ExpansionHubPidConstants m_velocityPidConstants;
  private final ExpansionHubPidConstants m_positionPidConstants;

  /**
   * Constructs a servo at the requested channel on a specific USB port.
   *
   * @param usbId The USB port ID the hub is connected to
   * @param channel The motor channel
   */
  public ExpansionHubMotor(int usbId, int channel) {
    m_hub = new ExpansionHub(usbId);
    m_channel = channel;

    if (!m_hub.checkMotorChannel(channel)) {
      m_hub.close();
      throw new IllegalArgumentException("Channel " + channel + " out of range");
    }

    if (!m_hub.checkAndReserveMotor(channel)) {
      m_hub.close();
      throw new AllocationException("ExpansionHub Motor already allocated");
    }

    m_hub.reportUsage("ExHubMotor[" + channel + "]", "ExHubMotor");

    NetworkTableInstance systemServer = SystemServer.getSystemServer();

    PubSubOption[] options =
        new PubSubOption[] {
          PubSubOption.sendAll(true),
          PubSubOption.keepDuplicates(true),
          PubSubOption.periodic(0.005)
        };

    m_encoderSubscriber =
        systemServer
            .getDoubleTopic("/rhsp/" + usbId + "/motor" + channel + "/encoder")
            .subscribe(0, options);
    m_encoderVelocitySubscriber =
        systemServer
            .getDoubleTopic("/rhsp/" + usbId + "/motor" + channel + "/encoderVelocity")
            .subscribe(0, options);
    m_currentSubscriber =
        systemServer
            .getDoubleTopic("/rhsp/" + usbId + "/motor" + channel + "/current")
            .subscribe(0, options);

    m_setpointPublisher =
        systemServer
            .getDoubleTopic("/rhsp/" + usbId + "/motor" + channel + "/setpoint")
            .publish(options);

    m_distancePerCountPublisher =
        systemServer
            .getDoubleTopic("/rhsp/" + usbId + "/motor" + channel + "/distancePerCount")
            .publish(options);

    m_floatOn0Publisher =
        systemServer
            .getBooleanTopic("/rhsp/" + usbId + "/motor" + channel + "/floatOn0")
            .publish(options);
    m_enabledPublisher =
        systemServer
            .getBooleanTopic("/rhsp/" + usbId + "/motor" + channel + "/enabled")
            .publish(options);

    m_modePublisher =
        systemServer
            .getIntegerTopic("/rhsp/" + usbId + "/motor" + channel + "/mode")
            .publish(options);

    m_reversedPublisher =
        systemServer
            .getBooleanTopic("/rhsp/" + usbId + "/motor" + channel + "/reversed")
            .publish(options);

    m_resetEncoderPublisher =
        systemServer
            .getBooleanTopic("/rhsp/" + usbId + "/motor" + channel + "/resetEncoder")
            .publish(options);

    m_velocityPidConstants = new ExpansionHubPidConstants(usbId, channel, true);
    m_positionPidConstants = new ExpansionHubPidConstants(usbId, channel, false);
  }

  /** Closes a motor so another instance can be constructed. */
  @Override
  public void close() {
    m_hub.unreserveMotor(m_channel);
    m_hub.close();
    m_hub = null;

    m_encoderSubscriber.close();
    m_encoderVelocitySubscriber.close();
    m_currentSubscriber.close();
    m_setpointPublisher.close();
    m_floatOn0Publisher.close();
    m_enabledPublisher.close();
    m_modePublisher.close();
    m_reversedPublisher.close();
    m_resetEncoderPublisher.close();
    m_distancePerCountPublisher.close();

    m_velocityPidConstants.close();
    m_positionPidConstants.close();
  }

  /**
   * Sets the percentage power to run the motor at, between -1 and 1.
   *
   * @param power The power to drive the motor at
   */
  public void setPercentagePower(double power) {
    m_modePublisher.set(kPercentageMode);
    m_setpointPublisher.set(power);
  }

  /**
   * Sets the voltage to run the motor at. This value will be continously scaled to match the input
   * voltage.
   *
   * @param voltage The voltage to drive the motor at
   */
  public void setVoltage(Voltage voltage) {
    m_modePublisher.set(kVoltageMode);
    m_setpointPublisher.set(voltage.in(Volts));
  }

  /**
   * Command the motor to drive to a specific position setpoint. This value will be scaled by
   * setDistancePerCount and influenced by the PID constants.
   *
   * @param setpoint The position setpoint to drive the motor to
   */
  public void setPositionSetpoint(double setpoint) {
    m_modePublisher.set(kPositionMode);
    m_setpointPublisher.set(setpoint);
  }

  /**
   * Command the motor to drive to a specific velocity setpoint. This value will be scaled by
   * setDistancePerCount and influenced by the PID constants.
   *
   * @param setpoint The velocity setpoint to drive the motor to
   */
  public void setVelocitySetpoint(double setpoint) {
    m_modePublisher.set(kVelocityMode);
    m_setpointPublisher.set(setpoint);
  }

  /**
   * Sets if the motor output is enabled or not. Defaults to false.
   *
   * @param enabled True to enable, false to disable
   */
  public void setEnabled(boolean enabled) {
    m_enabledPublisher.set(enabled);
  }

  /**
   * Sets if the motor should float or brake when 0 is commanded. Defaults to false.
   *
   * @param floatOn0 True to float when commanded 0, false to brake
   */
  public void setFloatOn0(boolean floatOn0) {
    m_floatOn0Publisher.set(floatOn0);
  }

  /**
   * Gets the current being pulled by the motor.
   *
   * @return Motor current
   */
  public Current getCurrent() {
    return Amps.of(m_currentSubscriber.get(0));
  }

  /**
   * Sets the distance per count of the encoder. Used to scale encoder readings.
   *
   * @param perCount The distance moved per count of the encoder
   */
  public void setDistancePerCount(double perCount) {
    m_distancePerCountPublisher.set(perCount);
  }

  /**
   * Gets if the underlying ExpansionHub is connected.
   *
   * @return True if hub is connected, otherwise false
   */
  public boolean isHubConnected() {
    return m_hub.isHubConnected();
  }

  /**
   * Gets the current velocity of the motor encoder. Scaled into distancePerCount units.
   *
   * @return Encoder velocity
   */
  public double getEncoderVelocity() {
    return m_encoderVelocitySubscriber.get(0);
  }

  /**
   * Gets the current position of the motor encoder. Scaled into distancePerCount units.
   *
   * @return Encoder position
   */
  public double getEncoderPosition() {
    return m_encoderSubscriber.get(0);
  }

  /**
   * Sets if the motor and encoder should be reversed.
   *
   * @param reversed True to reverse encoder, false otherwise
   */
  public void setReversed(boolean reversed) {
    m_reversedPublisher.set(reversed);
  }

  /** Reset the encoder count to 0. */
  public void resetEncoder() {
    m_resetEncoderPublisher.set(true);
  }

  /**
   * Gets the PID constants object for velocity PID.
   *
   * @return Velocity PID constants object
   */
  public ExpansionHubPidConstants getVelocityPidConstants() {
    return m_velocityPidConstants;
  }

  /**
   * Gets the PID constants object for position PID.
   *
   * @return Position PID constants object
   */
  public ExpansionHubPidConstants getPositionPidConstants() {
    return m_positionPidConstants;
  }
}
