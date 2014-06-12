/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimEncoder;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.util.BoundaryException;

/**
 * Class to read quad encoders.
 * Quadrature encoders are devices that count shaft rotation and can sense direction. The output of
 * the QuadEncoder class is an integer that can count either up or down, and can go negative for
 * reverse direction counting. When creating QuadEncoders, a direction is supplied that changes the
 * sense of the output to make code more readable if the encoder is mounted such that forward movement
 * generates negative values. Quadrature encoders have two digital outputs, an A Channel and a B Channel
 * that are out of phase with each other to allow the FPGA to do direction sensing.
 */
public class Encoder extends SensorBase implements CounterBase, PIDSource, LiveWindowSendable {
    private int m_index;
    private double m_distancePerPulse;		// distance of travel for each encoder tick
    private EncodingType m_encodingType = EncodingType.k4X;
    private boolean m_allocatedA;
    private boolean m_allocatedB;
    private boolean m_allocatedI;
    private PIDSourceParameter m_pidSource;
    private SimEncoder impl;

    /**
     * Common initialization code for Encoders.
     * This code allocates resources for Encoders and is common to all constructors.
     * @param reverseDirection If true, counts down instead of up (this is all relative)
     * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
     * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
     * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
     * a counter object will be used and the returned value will either exactly match the spec'd count
     * or be double (2x) the spec'd count.
     */
    private void initEncoder(int aSlot, int aChannel,
            int bSlot, int bChannel, boolean reverseDirection) {

    	LiveWindow.addSensor("Encoder", aSlot, aChannel, this);
        
        if ((bSlot < aSlot) || ((bSlot == aSlot) && (bChannel < aChannel))) { // Swap ports
        	int slot = bSlot;
        	int channel = bChannel;
        	bSlot = aSlot;
        	bChannel = aChannel;
        	aSlot = slot;
        	aChannel = channel;
        }
    	impl = new SimEncoder("simulator/dio/"+aSlot+"/"+aChannel+"/"+bSlot+"/"+bChannel);
    	setDistancePerPulse(1);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b modules and channels fully specified.
     * @param aSlot The a channel digital input module.
     * @param aChannel The a channel digital input channel.
     * @param bSlot The b channel digital input module.
     * @param bChannel The b channel digital input channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     */
    public Encoder(final int aSlot, final int aChannel,
            final int bSlot, final int bChannel,
            boolean reverseDirection) {
        m_allocatedA = true;
        m_allocatedB = true;
        m_allocatedI = false;
        initEncoder(aSlot, aChannel, bSlot, bChannel, reverseDirection);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b modules and channels fully specified.
     * @param aSlot The a channel digital input module.
     * @param aChannel The a channel digital input channel.
     * @param bSlot The b channel digital input module.
     * @param bChannel The b channel digital input channel.
     */
    public Encoder(final int aSlot, final int aChannel,
            final int bSlot, final int bChannel) {
        this(aSlot, aChannel, bSlot, bChannel, false);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b modules and channels fully specified.
     * @param aSlot The a channel digital input module.
     * @param aChannel The a channel digital input channel.
     * @param bSlot The b channel digital input module.
     * @param bChannel The b channel digital input channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
     * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
     * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
     * a counter object will be used and the returned value will either exactly match the spec'd count
     * or be double (2x) the spec'd count.
     */
    public Encoder(final int aSlot, final int aChannel,
            final int bSlot, final int bChannel,
            boolean reverseDirection, final EncodingType encodingType) {
        m_allocatedA = true;
        m_allocatedB = true;
        m_allocatedI = false;
        if (encodingType == null)
            throw new NullPointerException("Given encoding type was null");
        m_encodingType = encodingType;
        initEncoder(aSlot, aChannel, bSlot, bChannel, reverseDirection);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b modules and channels fully specified.
     * Using the index pulse forces 4x encoding.
     * @param aSlot The a channel digital input module.
     * @param aChannel The a channel digital input channel.
     * @param bSlot The b channel digital input module.
     * @param bChannel The b channel digital input channel.
     * @param indexSlot The index channel digital input module.
     * @param indexChannel The index channel digital input channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     */
    public Encoder(final int aSlot, final int aChannel,
            final int bSlot, final int bChannel, final int indexSlot,
            final int indexChannel,
            boolean reverseDirection) {
    	throw new UnsupportedOperationException("Simulation doesn't currently support indexed encoders.");
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b modules and channels fully specified.
     * Using the index pulse forces 4x encoding.
     * @param aSlot The a channel digital input module.
     * @param aChannel The a channel digital input channel.
     * @param bSlot The b channel digital input module.
     * @param bChannel The b channel digital input channel.
     * @param indexSlot The index channel digital input module.
     * @param indexChannel The index channel digital input channel.
     */
    public Encoder(final int aSlot, final int aChannel,
            final int bSlot, final int bChannel, final int indexSlot,
            final int indexChannel) {
    	throw new UnsupportedOperationException("Simulation doesn't currently support indexed encoders.");
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels assuming the default module.
     * @param aChannel The a channel digital input channel.
     * @param bChannel The b channel digital input channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     */
    public Encoder(final int aChannel, final int bChannel, boolean reverseDirection) {
        m_allocatedA = true;
        m_allocatedB = true;
        m_allocatedI = false;
        initEncoder(1, aChannel, 1, bChannel, reverseDirection);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels assuming the default module.
     * @param aChannel The a channel digital input channel.
     * @param bChannel The b channel digital input channel.
     */
    public Encoder(final int aChannel, final int bChannel) {
        this(aChannel, bChannel, false);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels assuming the default module.
     * @param aChannel The a channel digital input channel.
     * @param bChannel The b channel digital input channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
     * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
     * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
     * a counter object will be used and the returned value will either exactly match the spec'd count
     * or be double (2x) the spec'd count.
     */
    public Encoder(final int aChannel, final int bChannel, boolean reverseDirection, final EncodingType encodingType) {
        m_allocatedA = true;
        m_allocatedB = true;
        m_allocatedI = false;
        if (encodingType == null)
            throw new NullPointerException("Given encoding type was null");
        m_encodingType = encodingType;
        initEncoder(1, aChannel, 1, bChannel, reverseDirection);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels assuming the default module.
     * Using an index pulse forces 4x encoding
     * @param aChannel The a channel digital input channel.
     * @param bChannel The b channel digital input channel.
     * @param indexChannel The index channel digital input channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     */
    public Encoder(final int aChannel, final int bChannel, final int indexChannel, boolean reverseDirection) {
    	throw new UnsupportedOperationException("Simulation doesn't currently support indexed encoders.");
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels assuming the default module.
     * Using an index pulse forces 4x encoding
     * @param aChannel The a channel digital input channel.
     * @param bChannel The b channel digital input channel.
     * @param indexChannel The index channel digital input channel.
     */
    public Encoder(final int aChannel, final int bChannel, final int indexChannel) {
        this(aChannel, bChannel, indexChannel, false);
    }

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels as digital inputs. This is used in the case
     * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
     * and assume that they already are counted.
     * @param aSource The source that should be used for the a channel.
     * @param bSource the source that should be used for the b channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     */
    // Not Supported: public Encoder(DigitalSource aSource, DigitalSource bSource, boolean reverseDirection) {

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels as digital inputs. This is used in the case
     * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
     * and assume that they already are counted.
     * @param aSource The source that should be used for the a channel.
     * @param bSource the source that should be used for the b channel.
     */
    // Not Supported: public Encoder(DigitalSource aSource, DigitalSource bSource) {

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels as digital inputs. This is used in the case
     * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
     * and assume that they already are counted.
     * @param aSource The source that should be used for the a channel.
     * @param bSource the source that should be used for the b channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
     * selected, then an encoder FPGA object is used and the returned counts will be 4x the encoder
     * spec'd value since all rising and falling edges are counted. If 1X or 2X are selected then
     * a counter object will be used and the returned value will either exactly match the spec'd count
     * or be double (2x) the spec'd count.
     */
    // Not Supported: public Encoder(DigitalSource aSource, DigitalSource bSource, boolean reverseDirection, final EncodingType encodingType) {

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels as digital inputs. This is used in the case
     * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
     * and assume that they already are counted.
     * @param aSource The source that should be used for the a channel.
     * @param bSource the source that should be used for the b channel.
     * @param indexSource the source that should be used for the index channel.
     * @param reverseDirection represents the orientation of the encoder and inverts the output values
     * if necessary so forward represents positive values.
     */
    // Not Supported: public Encoder(DigitalSource aSource, DigitalSource bSource, DigitalSource indexSource, boolean reverseDirection) {

    /**
     * Encoder constructor.
     * Construct a Encoder given a and b channels as digital inputs. This is used in the case
     * where the digital inputs are shared. The Encoder class will not allocate the digital inputs
     * and assume that they already are counted.
     * @param aSource The source that should be used for the a channel.
     * @param bSource the source that should be used for the b channel.
     * @param indexSource the source that should be used for the index channel.
     */
    // Not Supported: public Encoder(DigitalSource aSource, DigitalSource bSource, DigitalSource indexSource) {

    public void free() {}

    /**
     * Start the Encoder.
     * Starts counting pulses on the Encoder device.
     */
    public void start() {
    	impl.start();
    }

    /**
     * Stops counting pulses on the Encoder device. The value is not changed.
     */
    public void stop() {
    	impl.stop();
    }

    /**
     * Gets the raw value from the encoder.
     * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
     * factor.
     * @return Current raw count from the encoder
     */
    public int getRaw() {
    	throw new UnsupportedOperationException("Simulation doesn't currently support raw values.");
    }

    /**
     * Gets the current count.
     * Returns the current count on the Encoder.
     * This method compensates for the decoding type.
     *
     * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale factor.
     */
    public int get() {
    	throw new UnsupportedOperationException("Simulation doesn't support ticks.");
    }

    /**
     * Reset the Encoder distance to zero.
     * Resets the current count to zero on the encoder.
     */
    public void reset() {
    	impl.reset();
    }

    /**
     * Returns the period of the most recent pulse.
     * Returns the period of the most recent Encoder pulse in seconds.
     * This method compensates for the decoding type.
     *
     * @deprecated Use getRate() in favor of this method.  This returns unscaled periods and getRate() scales using value from setDistancePerPulse().
     *
     * @return Period in seconds of the most recent pulse.
     */
    public double getPeriod() {
    	throw new UnsupportedOperationException("Simulation doesn't currently support deprecated methods.");
    }

    /**
     * Sets the maximum period for stopped detection.
     * Sets the value that represents the maximum period of the Encoder before it will assume
     * that the attached device is stopped. This timeout allows users to determine if the wheels or
     * other shaft has stopped rotating.
     * This method compensates for the decoding type.
     *
     *
     * @param maxPeriod The maximum time between rising and falling edges before the FPGA will
     * report the device stopped. This is expressed in seconds.
     */
    public void setMaxPeriod(double maxPeriod) {
    	// XXX: not implemented in simulation yet
    	throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
    }

    /**
     * Determine if the encoder is stopped.
     * Using the MaxPeriod value, a boolean is returned that is true if the encoder is considered
     * stopped and false if it is still moving. A stopped encoder is one where the most recent pulse
     * width exceeds the MaxPeriod.
     * @return True if the encoder is considered stopped.
     */
    public boolean getStopped() {
    	// XXX: not implemented in simulation yet
    	throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
    }

    /**
     * The last direction the encoder value changed.
     * @return The last direction the encoder value changed.
     */
    public boolean getDirection() {
    	// XXX: not implemented in simulation yet
    	throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
    }

    /**
     * Get the distance the robot has driven since the last reset.
     *
     * @return The distance driven since the last reset as scaled by the value from setDistancePerPulse().
     */
    public double getDistance() {
    	return m_distancePerPulse * impl.getPosition();
    }

    /**
     * Get the current rate of the encoder.
     * Units are distance per second as scaled by the value from setDistancePerPulse().
     *
     * @return The current rate of the encoder.
     */
    public double getRate() {
    	return m_distancePerPulse * impl.getVelocity();
    }

    /**
     * Set the minimum rate of the device before the hardware reports it stopped.
     *
     * @param minRate The minimum rate.  The units are in distance per second as scaled by the value from setDistancePerPulse().
     */
    public void setMinRate(double minRate) {
    	// XXX: not implemented in simulation yet
    	throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
    }

    /**
     * Set the distance per pulse for this encoder.
     * This sets the multiplier used to determine the distance driven based on the count value
     * from the encoder.
     * Do not include the decoding type in this scale.  The library already compensates for the decoding type.
     * Set this value based on the encoder's rated Pulses per Revolution and
     * factor in gearing reductions following the encoder shaft.
     * This distance can be in any units you like, linear or angular.
     *
     * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
     */
    public void setDistancePerPulse(double distancePerPulse) {
    	System.err.println("NOTE|WPILibJSim: Encoder.setDistancePerPulse() assumes 360 pulses per revolution in simulation.");
    	m_distancePerPulse = distancePerPulse;
    }

    /**
     * Set the direction sensing for this encoder.
     * This sets the direction sensing on the encoder so that it could count in the correct
     * software direction regardless of the mounting.
     * @param reverseDirection true if the encoder direction should be reversed
     */
    public void setReverseDirection(boolean reverseDirection) {
        // XXX: Currently uni-directional
    	throw new UnsupportedOperationException("Simulation doesn't support reversing direction.");
    }
    
    /**
     * Set the Samples to Average which specifies the number of samples of the timer to 
     * average when calculating the period. Perform averaging to account for 
     * mechanical imperfections or as oversampling to increase resolution.
     * @param samplesToAverage The number of samples to average from 1 to 127.
     */
    public void setSamplesToAverage(int samplesToAverage) {
    	throw new UnsupportedOperationException("Simulation doesn't support averaging samples.");
    }
    
    /**
     * Get the Samples to Average which specifies the number of samples of the timer to 
     * average when calculating the period. Perform averaging to account for 
     * mechanical imperfections or as oversampling to increase resolution.
     * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
     */
    public int getSamplesToAverage() {
    	throw new UnsupportedOperationException("Simulation doesn't support averaging samples.");
    }

    /**
     * Set which parameter of the encoder you are using as a process control variable.
     * The encoder class supports the rate and distance parameters.
     * @param pidSource An enum to select the parameter.
     */
    public void setPIDSourceParameter(PIDSourceParameter pidSource) {
        BoundaryException.assertWithinBounds(pidSource.value, 0, 1);
        m_pidSource = pidSource;
    }

    /**
     * Implement the PIDSource interface.
     *
     * @return The current value of the selected source parameter.
     */
    public double pidGet() {
        switch (m_pidSource.value) {
        case PIDSourceParameter.kDistance_val:
            return getDistance();
        case PIDSourceParameter.kRate_val:
            return getRate();
        default:
            return 0.0;
        }
    }

    /*
     * Live Window code, only does anything if live window is activated.
     */
    public String getSmartDashboardType(){
        switch(m_encodingType.value)
        {
            case EncodingType.k4X_val:
                return "Quadrature Encoder";
            default:
                return "Encoder";
        }
    }
    private ITable m_table;
    
    /**
     * {@inheritDoc}
     */
    public void initTable(ITable subtable) {
        m_table = subtable;
        updateTable();
    }
    
    /**
     * {@inheritDoc}
     */
    public ITable getTable(){
        return m_table;
    }
    
    /**
     * {@inheritDoc}
     */
    public void updateTable() {
        if (m_table != null) {
            m_table.putNumber("Speed", getRate());
            m_table.putNumber("Distance", getDistance());
            m_table.putNumber("Distance per Tick", m_distancePerPulse);
        }
    }

    /**
     * {@inheritDoc}
     */
    public void startLiveWindowMode() {}
    
    /**
     * {@inheritDoc}
     */
    public void stopLiveWindowMode() {}
}
