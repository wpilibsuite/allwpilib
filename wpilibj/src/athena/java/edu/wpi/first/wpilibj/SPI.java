/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.hal.SPIJNI;

/**
 * Represents a SPI bus port.
 */
public class SPI extends SensorBase {

  public enum Port {
    kOnboardCS0(0), kOnboardCS1(1), kOnboardCS2(2), kOnboardCS3(3), kMXP(4);

    @SuppressWarnings("MemberName")
    public int value;

    private Port(int value) {
      this.value = value;
    }
  }

  private static int devices = 0;

  private int m_port;
  private int m_bitOrder;
  private int m_clockPolarity;
  private int m_dataOnTrailing;

  /**
   * Constructor.
   *
   * @param port the physical SPI port
   */
  public SPI(Port port) {
    m_port = (byte) port.value;
    devices++;

    SPIJNI.spiInitialize(m_port);

    HAL.report(tResourceType.kResourceType_SPI, devices);
  }

  /**
   * Free the resources used by this object.
   */
  public void free() {
    SPIJNI.spiClose(m_port);
  }

  /**
   * Configure the rate of the generated clock signal. The default value is 500,000 Hz. The maximum
   * value is 4,000,000 Hz.
   *
   * @param hz The clock rate in Hertz.
   */
  public final void setClockRate(int hz) {
    SPIJNI.spiSetSpeed(m_port, hz);
  }

  /**
   * Configure the order that bits are sent and received on the wire to be most significant bit
   * first.
   */
  public final void setMSBFirst() {
    m_bitOrder = 1;
    SPIJNI.spiSetOpts(m_port, m_bitOrder, m_dataOnTrailing, m_clockPolarity);
  }

  /**
   * Configure the order that bits are sent and received on the wire to be least significant bit
   * first.
   */
  public final void setLSBFirst() {
    m_bitOrder = 0;
    SPIJNI.spiSetOpts(m_port, m_bitOrder, m_dataOnTrailing, m_clockPolarity);
  }

  /**
   * Configure the clock output line to be active low. This is sometimes called clock polarity high
   * or clock idle high.
   */
  public final void setClockActiveLow() {
    m_clockPolarity = 1;
    SPIJNI.spiSetOpts(m_port, m_bitOrder, m_dataOnTrailing, m_clockPolarity);
  }

  /**
   * Configure the clock output line to be active high. This is sometimes called clock polarity low
   * or clock idle low.
   */
  public final void setClockActiveHigh() {
    m_clockPolarity = 0;
    SPIJNI.spiSetOpts(m_port, m_bitOrder, m_dataOnTrailing, m_clockPolarity);
  }

  /**
   * Configure that the data is stable on the falling edge and the data changes on the rising edge.
   */
  public final void setSampleDataOnFalling() {
    m_dataOnTrailing = 1;
    SPIJNI.spiSetOpts(m_port, m_bitOrder, m_dataOnTrailing, m_clockPolarity);
  }

  /**
   * Configure that the data is stable on the rising edge and the data changes on the falling edge.
   */
  public final void setSampleDataOnRising() {
    m_dataOnTrailing = 0;
    SPIJNI.spiSetOpts(m_port, m_bitOrder, m_dataOnTrailing, m_clockPolarity);
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
   * Write data to the slave device. Blocks until there is space in the output FIFO.
   *
   * <p>If not running in output only mode, also saves the data received on the MISO input during
   * the transfer into the receive FIFO.
   */
  public int write(byte[] dataToSend, int size) {
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
    dataToSendBuffer.put(dataToSend);
    return SPIJNI.spiWrite(m_port, dataToSendBuffer, (byte) size);
  }

  /**
   * Write data to the slave device. Blocks until there is space in the output FIFO.
   *
   * <p>If not running in output only mode, also saves the data received on the MISO input during
   * the transfer into the receive FIFO.
   *
   * @param dataToSend The buffer containing the data to send. Must be created using
   *                   ByteBuffer.allocateDirect().
   */
  public int write(ByteBuffer dataToSend, int size) {
    if (!dataToSend.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (dataToSend.capacity() < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    return SPIJNI.spiWrite(m_port, dataToSend, (byte) size);
  }

  /**
   * Read a word from the receive FIFO.
   *
   * <p>Waits for the current transfer to complete if the receive FIFO is empty.
   *
   * <p>If the receive FIFO is empty, there is no active transfer, and initiate is false, errors.
   *
   * @param initiate If true, this function pushes "0" into the transmit buffer and initiates a
   *                 transfer. If false, this function assumes that data is already in the receive
   *                 FIFO from a previous write.
   */
  public int read(boolean initiate, byte[] dataReceived, int size) {
    final int retVal;
    ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(size);
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
    if (initiate) {
      retVal = SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceivedBuffer, (byte) size);
    } else {
      retVal = SPIJNI.spiRead(m_port, dataReceivedBuffer, (byte) size);
    }
    dataReceivedBuffer.get(dataReceived);
    return retVal;
  }

  /**
   * Read a word from the receive FIFO.
   *
   * <p>Waits for the current transfer to complete if the receive FIFO is empty.
   *
   * <p>If the receive FIFO is empty, there is no active transfer, and initiate is false, errors.
   *
   * @param initiate     If true, this function pushes "0" into the transmit buffer and initiates
   *                     a transfer. If false, this function assumes that data is already in the
   *                     receive FIFO from a previous write.
   * @param dataReceived The buffer to be filled with the received data. Must be created using
   *                     ByteBuffer.allocateDirect().
   * @param size         The length of the transaction, in bytes
   */
  public int read(boolean initiate, ByteBuffer dataReceived, int size) {
    if (!dataReceived.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (dataReceived.capacity() < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    if (initiate) {
      ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(size);
      return SPIJNI.spiTransaction(m_port, dataToSendBuffer, dataReceived, (byte) size);
    }
    return SPIJNI.spiRead(m_port, dataReceived, (byte) size);
  }

  /**
   * Perform a simultaneous read/write transaction with the device.
   *
   * @param dataToSend   The data to be written out to the device
   * @param dataReceived Buffer to receive data from the device
   * @param size         The length of the transaction, in bytes
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
   * @param dataToSend   The data to be written out to the device. Must be created using
   *                     ByteBuffer.allocateDirect().
   * @param dataReceived Buffer to receive data from the device. Must be created using
   *                     ByteBuffer.allocateDirect().
   * @param size         The length of the transaction, in bytes
   */
  public int transaction(ByteBuffer dataToSend, ByteBuffer dataReceived, int size) {
    if (!dataToSend.isDirect()) {
      throw new IllegalArgumentException("dataToSend must be a direct buffer");
    }
    if (dataToSend.capacity() < size) {
      throw new IllegalArgumentException("dataToSend is too small, must be at least " + size);
    }
    if (!dataReceived.isDirect()) {
      throw new IllegalArgumentException("dataReceived must be a direct buffer");
    }
    if (dataReceived.capacity() < size) {
      throw new IllegalArgumentException("dataReceived is too small, must be at least " + size);
    }
    return SPIJNI.spiTransaction(m_port, dataToSend, dataReceived, (byte) size);
  }

  /**
   * Initialize the accumulator.
   *
   * @param period     Time between reads
   * @param cmd        SPI command to send to request data
   * @param xferSize   SPI transfer size, in bytes
   * @param validMask  Mask to apply to received data for validity checking
   * @param validValue After validMask is applied, required matching value for validity checking
   * @param dataShift  Bit shift to apply to received data to get actual data value
   * @param dataSize   Size (in bits) of data field
   * @param isSigned   Is data field signed?
   * @param bigEndian  Is device big endian?
   */
  public void initAccumulator(double period, int cmd, int xferSize,
                              int validMask, int validValue,
                              int dataShift, int dataSize,
                              boolean isSigned, boolean bigEndian) {
    SPIJNI.spiInitAccumulator(m_port, (int) (period * 1.0e6), cmd,
        (byte) xferSize, validMask, validValue, (byte) dataShift,
        (byte) dataSize, isSigned, bigEndian);
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
   * <p>The center value is subtracted from each value before it is added to the accumulator. This
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
   * <p>Read the count of the accumulated values since the accumulator was last Reset().
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
   * <p>This function reads the value and count atomically. This can be used for averaging.
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
    ByteBuffer count = ByteBuffer.allocateDirect(8);
    // set the byte order
    count.order(ByteOrder.LITTLE_ENDIAN);
    SPIJNI.spiGetAccumulatorOutput(m_port, value.asLongBuffer(), count.asLongBuffer());
    result.value = value.asLongBuffer().get(0);
    result.count = count.asLongBuffer().get(0);
  }
}
