package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Class for supporting continuous analog encoders, such as the US Digital MA3.
 */
public class AnalogEncoder implements Sendable, AutoCloseable {
  private AnalogInput m_analogInput;
  private AnalogTrigger m_analogTrigger;
  private Counter m_counter;
  private double m_positionOffset = 0;
  private double m_distancePerRotation = 1.0;

  /**
   * Construct a new AnalogEncoder attached to a specific AnalogInput.
   * 
   * @param analogInput the analog input to attach to
   */
  public AnalogEncoder(AnalogInput analogInput) {
    m_analogInput = analogInput;
    init();
  }

  private void init() {
    m_analogTrigger = new AnalogTrigger(m_analogInput);
    m_counter = new Counter();
    // Limits need to be 25% from each end
    m_analogTrigger.setLimitsVoltage(1.25, 3.75);
    m_counter.setUpSource(m_analogTrigger, AnalogTriggerType.kRisingPulse);
    m_counter.setDownSource(m_analogTrigger, AnalogTriggerType.kFallingPulse);

    SendableRegistry.addLW(this, "Analog Encoder", m_analogInput.getChannel());
  }

  /**
   * Get the number of whole rotations since the last reset.
   * 
   * @return number of whole rotations
   */
  public int getRotations() {
    return m_counter.get();
  }

  /**
   * Get the encoder value since the last reset.
   * 
   * <p>This is reported in rotations since the last reset.
   * 
   * @return the encoder value in rotations
   */
  public double get() {
    return getRotations() + getPositionInRotation() - m_positionOffset;
  }

  /**
   * Get the absolute position in the rotation.
   * 
   * <p>This is not affected by reset(), and is always just the absolute value straight
   * from the encoder.
   * 
   * @return the encoder absolute position
   */
  public double getPositionInRotation() {
    return m_analogInput.getVoltage() / 5.0;
  }

  /**
   * Get the offset of position relative to the last reset.
   * 
   * <p>getPositionInRotation() - getPositionOffset() will give an encoder absolute position
   * relative to the last reset. This could potentially be negative, which needs to be accounted
   * for.
   * 
   * @return the position offset
   */
  public double getPositionOffset() {
    return m_positionOffset;
  }

  /**
   * Set the distance per rotation of the encoder. This sets the multiplier used to determine the
   * distance driven based on the rotation value from the encoder. Set this value based on 
   * the how far the mechanism travels in 1 rotation of the encoder, and factor in gearing 
   * reductions following the encoder shaft. This distance can be in any units you like, 
   * linear or angular.
   * 
   * @param distancePerRotation the distance per rotation of the encoder
   */
  public void setDistancePerRotation(double distancePerRotation) {
    m_distancePerRotation = distancePerRotation;
  }

  /**
   * Get the distance per rotation for this encoder.
   *
   * @return The scale factor that will be used to convert rotation to useful units.
   */
  public double getDistancePerRotation() {
    return m_distancePerRotation;
  }

  /**
   * Get the distance the sensor has driven since the last reset as scaled by the value from {@link
   * #setDistancePerRotation(double)}.
   *
   * @return The distance driven since the last reset
   */
  public double getDistance() {
    return get() * getDistancePerRotation();
  }

  /**
   * Reset the Encoder distance to zero.
   */
  public void reset() {
    m_counter.reset();
    m_positionOffset = getPositionInRotation();
  }

  @Override
  public void close() {
    m_counter.close();
    m_analogTrigger.close();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("AbsoluteEncoder");
    builder.addDoubleProperty("Distance", this::getDistance, null);
    builder.addDoubleProperty("Distance Per Rotation", this::getDistancePerRotation, null);

  }
  
  

}
