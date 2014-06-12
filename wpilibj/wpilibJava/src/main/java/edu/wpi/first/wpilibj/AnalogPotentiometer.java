package edu.wpi.first.wpilibj;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class for reading analog potentiometers. Analog potentiometers read
 * in an analog voltage that corresponds to a position. Usually the
 * position is either degrees or meters. However, if no conversion is
 * given it remains volts.
 *
 * @author Alex Henning
 */
public class AnalogPotentiometer implements Potentiometer, LiveWindowSendable {
    private int m_module, m_channel;
    private double m_scale, m_offset;
    private AnalogInput m_analog_input;
    
    /**
     * Common initialization code called by all constructors.
     */
    private void initPot(final int slot, final int channel, double scale, double offset) {
        this.m_module = slot;
        this.m_channel = channel;
        this.m_scale = scale;
        this.m_offset = offset;
        m_analog_input = new AnalogInput(slot, channel);
    }
    
    /**
     * AnalogPotentiometer constructor.
     *
     * Use the scaling and offset values so that the output produces
     * meaningful values. I.E: you have a 270 degree potentiometer and
     * you want the output to be degrees with the halfway point as 0
     * degrees. The scale value is 270.0(degrees)/5.0(volts) and the
     * offset is -135.0 since the halfway point after scaling is 135
     * degrees.
     *
     * @param slot The analog module this potentiometer is plugged into.
     * @param channel The analog channel this potentiometer is plugged into.
     * @param scale The scaling to multiply the voltage by to get a meaningful unit.
     * @param offset The offset to add to the scaled value for controlling the zero value
     */
    public AnalogPotentiometer(final int slot, final int channel, double scale, double offset) {
        initPot(slot, channel, scale, offset);
    }
    
    /**
     * AnalogPotentiometer constructor.
     *
     * Use the scaling and offset values so that the output produces
     * meaningful values. I.E: you have a 270 degree potentiometer and
     * you want the output to be degrees with the halfway point as 0
     * degrees. The scale value is 270.0(degrees)/5.0(volts) and the
     * offset is -135.0 since the halfway point after scaling is 135
     * degrees.
     *
     * @param channel The analog channel this potentiometer is plugged into.
     * @param scale The scaling to multiply the voltage by to get a meaningful unit.
     * @param offset The offset to add to the scaled value for controlling the zero value
     */
    public AnalogPotentiometer(final int channel, double scale, double offset) {
        initPot(1, channel, scale, offset);
    }
    
    /**
     * AnalogPotentiometer constructor.
     *
     * Use the scaling and offset values so that the output produces
     * meaningful values. I.E: you have a 270 degree potentiometer and
     * you want the output to be degrees with the halfway point as 0
     * degrees. The scale value is 270.0(degrees)/5.0(volts) and the
     * offset is -135.0 since the halfway point after scaling is 135
     * degrees.
     *
     * @param channel The analog channel this potentiometer is plugged into.
     * @param scale The scaling to multiply the voltage by to get a meaningful unit.
     */
    public AnalogPotentiometer(final int channel, double scale) {
        initPot(1, channel, scale, 0);
    }
    
    /**
     * AnalogPotentiometer constructor.
     *
     * @param channel The analog channel this potentiometer is plugged into.
     */
    public AnalogPotentiometer(final int channel) {
        initPot(1, channel, 1, 0);
    }
    
    /**
     * Get the current reading of the potentiomere.
     *
     * @return The current position of the potentiometer.
     */
    public double get() {
        return m_analog_input.getVoltage() * m_scale + m_offset;
    }
    
    
    /**
     * Implement the PIDSource interface.
     *
     * @return The current reading.
     */
    public double pidGet() {
        return get();
    }
    
    /*
     * Live Window code, only does anything if live window is activated.
     */
    public String getSmartDashboardType(){
        return "Analog Input";
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
    public void updateTable() {
        if (m_table != null) {
            m_table.putNumber("Value", get());
        }
    }
    
    /**
     * {@inheritDoc}
     */
    public ITable getTable(){
        return m_table;
    }
    
    /**
     * Analog Channels don't have to do anything special when entering the LiveWindow.
     * {@inheritDoc}
     */
    public void startLiveWindowMode() {}
    
    /**
     * Analog Channels don't have to do anything special when exiting the LiveWindow.
     * {@inheritDoc}
     */
    public void stopLiveWindowMode() {}
}
