package edu.wpi.first.wpilibj;

import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.SPIJNI;

/**
 *
 * Represents a SPI bus port
 
 * @author koconnor
 */
public class SPI extends SensorBase {
	
	public enum Port {
		kOnboardCS0(0), 
		kOnboardCS1(1), 
		kOnboardCS2(2), 
		kOnboardCS3(3), 
		kMXP(4);
		
		private int value;
		
		private Port(int value){
			this.value = value;
		}
		
		public int getValue(){
			return this.value;
		}
	};

	private static int devices = 0;
	
	private byte m_port;
	private int bitOrder;
	private int clockPolarity;
	private int dataOnTrailing;

	/**
	 * Constructor
	 *
	 * @param port the physical SPI port
	 */
    public SPI(Port port) {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		m_port = (byte)port.getValue();
		devices++;
		
        SPIJNI.spiInitialize(m_port, status.asIntBuffer());
		HALUtil.checkStatus(status.asIntBuffer());
		
		UsageReporting.report(tResourceType.kResourceType_SPI, devices);
    }  
        
    /**
     * Free the resources used by this object
     */
    public void free(){
		SPIJNI.spiClose(m_port);
    }
	
	/**
	 * Configure the rate of the generated clock signal.
	 * The default value is 500,000 Hz.
	 * The maximum value is 4,000,000 Hz.
	 *
	 * @param hz	The clock rate in Hertz.
	 */
    public final void setClockRate(int hz) {
        SPIJNI.spiSetSpeed(m_port, hz);
    }
	
	/**
	 * Configure the order that bits are sent and received on the wire
	 * to be most significant bit first.
	 */
	public final void setMSBFirst() {
		this.bitOrder = 1;
		SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
	}
	
	/**
	 * Configure the order that bits are sent and received on the wire
	 * to be least significant bit first.
	 */
	public final void setLSBFirst() {
		this.bitOrder = 0;
		SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
	}

	/**
	 * Configure the clock output line to be active low.
	 * This is sometimes called clock polarity high or clock idle high.
	 */
	public final void setClockActiveLow() {
		this.clockPolarity = 1;
		SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
	}
	
	/**
	 * Configure the clock output line to be active high.
	 * This is sometimes called clock polarity low or clock idle low.
	 */
	public final void setClockActiveHigh() {
		this.clockPolarity = 0;
		SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
	}

	/**
	 * Configure that the data is stable on the falling edge and the data
	 * changes on the rising edge.
	 */
    public final void setSampleDataOnFalling() {
        this.dataOnTrailing = 1;
		SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
    }
	
	/**
	 * Configure that the data is stable on the rising edge and the data
	 * changes on the falling edge.
	 */
	public final void setSampleDataOnRising() {
        this.dataOnTrailing = 0;
		SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
    }
	
	/**
	 * Configure the chip select line to be active high.
	 */
	public final void setChipSelectActiveHigh() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		SPIJNI.spiSetChipSelectActiveHigh(m_port, status.asIntBuffer());
	
		HALUtil.checkStatus(status.asIntBuffer());
	}
	
	/**
	 * Configure the chip select line to be active low.
	 */
	public final void setChipSelectActiveLow() {
		ByteBuffer status = ByteBuffer.allocateDirect(4);
		status.order(ByteOrder.LITTLE_ENDIAN);
		
		SPIJNI.spiSetChipSelectActiveLow(m_port, status.asIntBuffer());
	
		HALUtil.checkStatus(status.asIntBuffer());
	}
	
	 /**
	 * Write data to the slave device.  Blocks until there is space in the
	 * output FIFO.
	 *
	 * If not running in output only mode, also saves the data received
	 * on the MISO input during the transfer into the receive FIFO.
	 */
	public int write(byte[] dataToSend, int size) {
		int retVal = 0;
		ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
		dataToSendBuffer.put(dataToSend);
		retVal = SPIJNI.spiWrite(m_port, dataToSendBuffer, (byte) size);
		return retVal;
	}
	
	/**
	 * Read a word from the receive FIFO.
	 *
	 * Waits for the current transfer to complete if the receive FIFO is empty.
	 *
	 * If the receive FIFO is empty, there is no active transfer, and initiate
	 * is false, errors.
	 *
	 * @param initiate	If true, this function pushes "0" into the
	 *				    transmit buffer and initiates a transfer.
	 *				    If false, this function assumes that data is
	 *				    already in the receive FIFO from a previous write.
	 */
	public int read(boolean initiate, byte[] dataReceived, int size) {
		int retVal = 0;
		ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(size);
		ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
		if(initiate)
			retVal = SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceivedBuffer, (byte) size);
		else
			retVal = SPIJNI.spiRead(m_port, dataReceivedBuffer, (byte) size);
		dataReceivedBuffer.get(dataReceived);
		return retVal;
	}

	/**
	 * Perform a simultaneous read/write transaction with the device
	 *
	 * @param dataToSend The data to be written out to the device
	 * @param dataReceived Buffer to receive data from the device
	 * @param size The length of the transaction, in bytes
	 */
    public int transaction(byte[] dataToSend, byte[] dataReceived, int size) {
		int retVal = 0;
		ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
		dataToSendBuffer.put(dataToSend);
		ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(size);
        retVal = SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceivedBuffer, (byte) size);
		dataReceivedBuffer.get(dataReceived);
        return retVal;
    }
}
