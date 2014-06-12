package edu.wpi.first.wpilibj;
import edu.wpi.first.wpilibj.interfaces.Potentiometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimFloatInput;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * A class for reading analog potentiometers.
 * 
 * @author Alex Henning
 */
public class AnalogPotentiometer implements Potentiometer, LiveWindowSendable {
	private int module, channel;
	private SimFloatInput impl;

    /**
     * Common initialization code called by all constructors.
     */
    private void initPot(final int slot, final int channel, double scale, double offset) {
    	this.module = slot;
    	this.channel = channel;
    	impl = new SimFloatInput("simulator/analog/"+slot+"/"+channel);
    }
    
    public AnalogPotentiometer(final int slot, final int channel, double scale, double offset) {
    	initPot(slot, channel, scale, offset);
    }

    public AnalogPotentiometer(final int channel, double scale, double offset) {
    	initPot(1, channel, scale, offset);
    }

    public AnalogPotentiometer(final int channel, double scale) {
    	initPot(1, channel, scale, 0);
    }

    public AnalogPotentiometer(final int channel) {
    	initPot(1, channel, 1, 0);
    }

	@Override
	public double get() {
		return impl.get();
	}
    
	@Override
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
