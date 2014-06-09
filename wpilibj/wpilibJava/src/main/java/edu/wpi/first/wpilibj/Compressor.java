package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.SensorBase;
import edu.wpi.first.wpilibj.hal.CompressorJNI;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.parsing.IDevice;
import edu.wpi.first.wpilibj.tables.ITable;

public class Compressor extends SensorBase implements IDevice, LiveWindowSendable {
	private ByteBuffer m_pcm;
	
	public Compressor(int module) {
		initCompressor(module);
	}
	
	public Compressor() {
		initCompressor(getDefaultSolenoidModule());
	}
	
	private void initCompressor(int module) {
		m_table = null;
		
		m_pcm = CompressorJNI.initializeCompressor((byte)module);
	}
	
	public void start() {
		setClosedLoopControl(false);
		setCompressor(true);
	}
	
	public void stop() {
		setClosedLoopControl(false);
		setCompressor(false);
	}
	
	public boolean enabled() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		boolean on = CompressorJNI.getCompressor(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		
		return on;
	}
	
	public boolean getPressureSwitchValue() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		boolean on = CompressorJNI.getPressureSwitch(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		
		return on;
	}
	
	public float getCompressorCurrent() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		float current = CompressorJNI.getCompressorCurrent(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		
		return current;
	}
	
	public void setClosedLoopControl(boolean on) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		CompressorJNI.setClosedLoopControl(m_pcm, on, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}
	
	public boolean getClosedLoopControl() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		boolean on = CompressorJNI.getClosedLoopControl(m_pcm, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		
		return on;
	}

	@Override
	public void startLiveWindowMode() {
	}

	@Override
	public void stopLiveWindowMode() {
	}
	
	private void setCompressor(boolean on) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		CompressorJNI.setCompressor(m_pcm, on, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
	}
	
	@Override
	public String getSmartDashboardType() {
		return "Compressor";
	}

	private ITable m_table;

	@Override
	public void initTable(ITable subtable) {
		m_table = subtable;
		updateTable();
	}
	
	@Override
	public ITable getTable() {
		return m_table;
	}

	@Override
	public void updateTable() {
        if (m_table != null) {
            m_table.putBoolean("Enabled", enabled());
            m_table.putBoolean("Pressure Switch", getPressureSwitchValue());
        }
	}
}
