/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.nio.LongBuffer;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.SPIJNI;

/**
 *
 * Represents a SPI bus port
 *$
 * @author koconnor
 */
public class SPI extends SensorBase {

  public enum Port {
    kOnboardCS0(0), kOnboardCS1(1), kOnboardCS2(2), kOnboardCS3(3), kMXP(4);

    private int value;

    private Port(int value) {
      this.value = value;
    }

    public int getValue() {
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
    m_port = (byte) port.getValue();
    devices++;

    SPIJNI.spiInitialize(m_port);

    UsageReporting.report(tResourceType.kResourceType_SPI, devices);
  }

  /**
   * Free the resources used by this object
   */
  public void free() {
    SPIJNI.spiClose(m_port);
  }

  /**
   * Configure the rate of the generated clock signal. The default value is
   * 500,000 Hz. The maximum value is 4,000,000 Hz.
   *
   * @param hz The clock rate in Hertz.
   */
  public final void setClockRate(int hz) {
    SPIJNI.spiSetSpeed(m_port, hz);
  }

  /**
   * Configure the order that bits are sent and received on the wire to be most
   * significant bit first.
   */
  public final void setMSBFirst() {
    this.bitOrder = 1;
    SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
  }

  /**
   * Configure the order that bits are sent and received on the wire to be least
   * significant bit first.
   */
  public final void setLSBFirst() {
    this.bitOrder = 0;
    SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
  }

  /**
   * Configure the clock output line to be active low. This is sometimes called
   * clock polarity high or clock idle high.
   */
  public final void setClockActiveLow() {
    this.clockPolarity = 1;
    SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
  }

  /**
   * Configure the clock output line to be active high. This is sometimes called
   * clock polarity low or clock idle low.
   */
  public final void setClockActiveHigh() {
    this.clockPolarity = 0;
    SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
  }

  /**
   * Configure that the data is stable on the falling edge and the data changes
   * on the rising edge.
   */
  public final void setSampleDataOnFalling() {
    this.dataOnTrailing = 1;
    SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
  }

  /**
   * Configure that the data is stable on the rising edge and the data changes
   * on the falling edge.
   */
  public final void setSampleDataOnRising() {
    this.dataOnTrailing = 0;
    SPIJNI.spiSetOpts(m_port, this.bitOrder, this.dataOnTrailing, this.clockPolarity);
  }

  /**
   * Configure the chip select line to be active high.
   */
  public final void setChipSelectActiveHigh() {
    SPIJNI.spiSetChipSelectActiveHigh(m_port);
  }

  /**
   * Configure the chip select line to be active low.
   */
  public final void setChipSelectActiveLow() {
    SPIJNI.spiSetChipSelectActiveLow(m_port);
  }

  /**
   * Write data to the slave device. Blocks until there is space in the output
   * FIFO.
   *
   * If not running in output only mode, also saves the data received on the
   * MISO input during the transfer into the receive FIFO.
   */
  public int write(byte[] dataToSend, int size) {
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
    dataToSendBuffer.put(dataToSend);
    return SPIJNI.spiWrite(m_port, dataToSendBuffer, (byte) size);
  }

  /**
   * Write data to the slave device. Blocks until there is space in the output
   * FIFO.
   *
   * If not running in output only mode, also saves the data received on the
   * MISO input during the transfer into the receive FIFO.
   *
   * @param dataToSend The buffer containing the data to send. Must be created
   *        using ByteBuffer.allocateDirect().
   */
  public int write(ByteBuffer dataToSend, int size) {
    if (!dataToSend.isDirect())
      throw new IllegalArgumentException("must be a direct buffer");
    if (dataToSend.capacity() < size)
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    return SPIJNI.spiWrite(m_port, dataToSend, (byte) size);
  }

  /**
   * Read a word from the receive FIFO.
   *
   * Waits for the current transfer to complete if the receive FIFO is empty.
   *
   * If the receive FIFO is empty, there is no active transfer, and initiate is
   * false, errors.
   *
   * @param initiate If true, this function pushes "0" into the transmit buffer
   *        and initiates a transfer. If false, this function assumes that data
   *        is already in the receive FIFO from a previous write.
   */
  public int read(boolean initiate, byte[] dataReceived, int size) {
    int retVal = 0;
    ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(size);
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
    if (initiate)
      retVal = SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceivedBuffer, (byte) size);
    else
      retVal = SPIJNI.spiRead(m_port, dataReceivedBuffer, (byte) size);
    dataReceivedBuffer.get(dataReceived);
    return retVal;
  }

  /**
   * Read a word from the receive FIFO.
   *
   * Waits for the current transfer to complete if the receive FIFO is empty.
   *
   * If the receive FIFO is empty, there is no active transfer, and initiate is
   * false, errors.
   *
   * @param initiate If true, this function pushes "0" into the transmit buffer
   *        and initiates a transfer. If false, this function assumes that data
   *        is already in the receive FIFO from a previous write.
   *
   * @param dataReceived The buffer to be filled with the received data. Must be
   *        created using ByteBuffer.allocateDirect().
   *
   * @param size The length of the transaction, in bytes
   */
  public int read(boolean initiate, ByteBuffer dataReceived, int size) {
    if (!dataReceived.isDirect())
      throw new IllegalArgumentException("must be a direct buffer");
    if (dataReceived.capacity() < size)
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    if (initiate) {
      ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
      return SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceived, (byte) size);
    }
    return SPIJNI.spiRead(m_port, dataReceived, (byte) size);
  }

  /**
   * Perform a simultaneous read/write transaction with the device
   *
   * @param dataToSend The data to be written out to the device
   * @param dataReceived Buffer to receive data from the device
   * @param size The length of the transaction, in bytes
   */
  public int transaction(byte[] dataToSend, byte[] dataReceived, int size) {
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
    dataToSendBuffer.put(dataToSend);
    ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(size);
    int retVal = SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceivedBuffer, (byte) size);
    dataReceivedBuffer.get(dataReceived);
    return retVal;
  }

  /**
   * Perform a simultaneous read/write transaction with the device
   *
   * @param dataToSend The data to be written out to the device. Must be
   *        created using ByteBuffer.allocateDirect().
   * @param dataReceived Buffer to receive data from the device. Must be
   *        created using ByteBuffer.allocateDirect().
   * @param size The length of the transaction, in bytes
   */
  public int transaction(ByteBuffer dataToSend, ByteBuffer dataReceived, int size) {
    if (!dataToSend.isDirect())
      throw new IllegalArgumentException("dataToSend must be a direct buffer");
    if (dataToSend.capacity() < size)
      throw new IllegalArgumentException("dataToSend is too small, must be at least " + size);
    if (!dataReceived.isDirect())
      throw new IllegalArgumentException("dataReceived must be a direct buffer");
    if (dataReceived.capacity() < size)
      throw new IllegalArgumentException("dataReceived is too small, must be at least " + size);
    return SPIJNI.spiTransaction(m_port, dataToSend, dataReceived, (byte) size);
  }

  /**
   * Initialize the accumulator.
   *
   * @param period Time between reads
   * @param cmd SPI command to send to request data
   * @param xfer_size SPI transfer size, in bytes
   * @param valid_mask Mask to apply to received data for validity checking
   * @param valid_data After valid_mask is applied, required matching value for
   *                   validity checking
   * @param data_shift Bit shift to apply to received data to get actual data
   *                   value
   * @param data_size Size (in bits) of data field
   * @param is_signed Is data field signed?
   * @param big_endian Is device big endian?
   */
  public void initAccumulator(double period, int cmd, int xfer_size,
                              int valid_mask, int valid_value,
                              int data_shift, int data_size,
                              boolean is_signed, boolean big_endian) {
    SPIJNI.spiInitAccumulator(m_port, (int)(period * 1.0e6), cmd,
        (byte)xfer_size, valid_mask, valid_value, (byte)data_shift,
        (byte)data_size, is_signed, big_endian);
  }

  /**
   * Frees the accumulator.
   */
  public void freeAccumulator() {
    SPIJNI.spiFreeAccumulator(m_port);
  }

  /**
   * Resets the accumulator to zero.
   */
  public void resetAccumulator() {
    SPIJNI.spiResetAccumulator(m_port);
  }

  /**
   * Set the center value of the accumulator.
   *
   * The center value is subtracted from each value before it is added to the accumulator. This
   * is used for the center value of devices like gyros and accelerometers to make integration work
   * and to take the device offset into account when integrating.
   */
  public void setAccumulatorCenter(int center) {
    SPIJNI.spiSetAccumulatorCenter(m_port, center);
  }

  /**
   * Set the accumulator's deadband.
   */
  public void setAccumulatorDeadband(int deadband) {
    SPIJNI.spiSetAccumulatorDeadband(m_port, deadband);
  }

  /**
   * Read the last value read by the accumulator engine.
   */
  public int getAccumulatorLastValue() {
    return SPIJNI.spiGetAccumulatorLastValue(m_port);
  }

  /**
   * Read the accumulated value.
   *
   * @return The 64-bit value accumulated since the last Reset().
   */
  public long getAccumulatorValue() {
    return SPIJNI.spiGetAccumulatorValue(m_port);
  }

  /**
   * Read the number of accumulated values.
   *
   * Read the count of the accumulated values since the accumulator was last Reset().
   *
   * @return The number of times samples from the channel were accumulated.
   */
  public int getAccumulatorCount() {
    return SPIJNI.spiGetAccumulatorCount(m_port);
  }

  /**
   * Read the average of the accumulated value.
   *
   * @return The accumulated average value (value / count).
   */
  public double getAccumulatorAverage() {
    return SPIJNI.spiGetAccumulatorAverage(m_port);
  }

  /**
   * Read the accumulated value and the number of accumulated values atomically.
   *
   * This function reads the value and count atomically.
   * This can be used for averaging.
   *
   * @param result AccumulatorResult object to store the results in.
   */
  public void getAccumulatorOutput(AccumulatorResult result) {
    if (result == null) {
      throw new IllegalArgumentException("Null parameter `result'");
    }
    ByteBuffer value = ByteBuffer.allocateDirect(8);
    // set the byte order
    value.order(ByteOrder.LITTLE_ENDIAN);
    ByteBuffer count = ByteBuffer.allocateDirect(4);
    // set the byte order
    count.order(ByteOrder.LITTLE_ENDIAN);
    SPIJNI.spiGetAccumulatorOutput(m_port, value.asLongBuffer(), count.asIntBuffer());
    result.value = value.asLongBuffer().get(0);
    result.count = count.asIntBuffer().get(0);
  }
}
