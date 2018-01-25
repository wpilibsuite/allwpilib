/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.hal.SPIJNI;

/**
 * Represents a SPI bus port.
 */
public class SPI {
  public enum Port {
    kOnboardCS0(0), kOnboardCS1(1), kOnboardCS2(2), kOnboardCS3(3), kMXP(4);

    @SuppressWarnings("MemberName")
    public int value;

    Port(int value) {
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
    if (m_accum != null) {
      m_accum.free();
      m_accum = null;
    }
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
    if (dataToSend.length < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    return SPIJNI.spiWriteB(m_port, dataToSend, (byte) size);
  }

  /**
   * Write data to the slave device. Blocks until there is space in the output FIFO.
   *
   * <p>If not running in output only mode, also saves the data received on the MISO input during
   * the transfer into the receive FIFO.
   *
   * @param dataToSend The buffer containing the data to send.
   */
  public int write(ByteBuffer dataToSend, int size) {
    if (dataToSend.hasArray()) {
      return write(dataToSend.array(), size);
    }
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
    if (dataReceived.length < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    return SPIJNI.spiReadB(m_port, initiate, dataReceived, (byte) size);
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
   * @param dataReceived The buffer to be filled with the received data.
   * @param size         The length of the transaction, in bytes
   */
  public int read(boolean initiate, ByteBuffer dataReceived, int size) {
    if (dataReceived.hasArray()) {
      return read(initiate, dataReceived.array(), size);
    }
    if (!dataReceived.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (dataReceived.capacity() < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    return SPIJNI.spiRead(m_port, initiate, dataReceived, (byte) size);
  }

  /**
   * Perform a simultaneous read/write transaction with the device.
   *
   * @param dataToSend   The data to be written out to the device
   * @param dataReceived Buffer to receive data from the device
   * @param size         The length of the transaction, in bytes
   */
  public int transaction(byte[] dataToSend, byte[] dataReceived, int size) {
    if (dataToSend.length < size) {
      throw new IllegalArgumentException("dataToSend is too small, must be at least " + size);
    }
    if (dataReceived.length < size) {
      throw new IllegalArgumentException("dataReceived is too small, must be at least " + size);
    }
    return SPIJNI.spiTransactionB(m_port, dataToSend, dataReceived, (byte) size);
  }

  /**
   * Perform a simultaneous read/write transaction with the device.
   *
   * @param dataToSend   The data to be written out to the device.
   * @param dataReceived Buffer to receive data from the device.
   * @param size         The length of the transaction, in bytes
   */
  public int transaction(ByteBuffer dataToSend, ByteBuffer dataReceived, int size) {
    if (dataToSend.hasArray() && dataReceived.hasArray()) {
      return transaction(dataToSend.array(), dataReceived.array(), size);
    }
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
   * Initialize automatic SPI transfer engine.
   *
   * <p>Only a single engine is available, and use of it blocks use of all other
   * chip select usage on the same physical SPI port while it is running.
   *
   * @param bufferSize buffer size in bytes
   */
  public void initAuto(int bufferSize) {
    SPIJNI.spiInitAuto(m_port, bufferSize);
  }

  /**
   * Frees the automatic SPI transfer engine.
   */
  public void freeAuto() {
    SPIJNI.spiFreeAuto(m_port);
  }

  /**
   * Set the data to be transmitted by the engine.
   *
   * <p>Up to 16 bytes are configurable, and may be followed by up to 127 zero
   * bytes.
   *
   * @param dataToSend data to send (maximum 16 bytes)
   * @param zeroSize number of zeros to send after the data
   */
  public void setAutoTransmitData(byte[] dataToSend, int zeroSize) {
    SPIJNI.spiSetAutoTransmitData(m_port, dataToSend, zeroSize);
  }

  /**
   * Start running the automatic SPI transfer engine at a periodic rate.
   *
   * <p>{@link #initAuto(int)} and {@link #setAutoTransmitData(byte[], int)} must
   * be called before calling this function.
   *
   * @param period period between transfers, in seconds (us resolution)
   */
  public void startAutoRate(double period) {
    SPIJNI.spiStartAutoRate(m_port, period);
  }

  /**
   * Start running the automatic SPI transfer engine when a trigger occurs.
   *
   * <p>{@link #initAuto(int)} and {@link #setAutoTransmitData(byte[], int)} must
   * be called before calling this function.
   *
   * @param source digital source for the trigger (may be an analog trigger)
   * @param rising trigger on the rising edge
   * @param falling trigger on the falling edge
   */
  public void startAutoTrigger(DigitalSource source, boolean rising, boolean falling) {
    SPIJNI.spiStartAutoTrigger(m_port, source.getPortHandleForRouting(),
                               source.getAnalogTriggerTypeForRouting(), rising, falling);
  }

  /**
   * Stop running the automatic SPI transfer engine.
   */
  public void stopAuto() {
    SPIJNI.spiStopAuto(m_port);
  }

  /**
   * Force the engine to make a single transfer.
   */
  public void forceAutoRead() {
    SPIJNI.spiForceAutoRead(m_port);
  }

  /**
   * Read data that has been transferred by the automatic SPI transfer engine.
   *
   * <p>Transfers may be made a byte at a time, so it's necessary for the caller
   * to handle cases where an entire transfer has not been completed.
   *
   * <p>Blocks until numToRead bytes have been read or timeout expires.
   * May be called with numToRead=0 to retrieve how many bytes are available.
   *
   * @param buffer buffer where read bytes are stored
   * @param numToRead number of bytes to read
   * @param timeout timeout in seconds (ms resolution)
   * @return Number of bytes remaining to be read
   */
  public int readAutoReceivedData(ByteBuffer buffer, int numToRead, double timeout) {
    if (buffer.hasArray()) {
      return readAutoReceivedData(buffer.array(), numToRead, timeout);
    }
    if (!buffer.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (buffer.capacity() < numToRead) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + numToRead);
    }
    return SPIJNI.spiReadAutoReceivedData(m_port, buffer, numToRead, timeout);
  }

  /**
   * Read data that has been transferred by the automatic SPI transfer engine.
   *
   * <p>Transfers may be made a byte at a time, so it's necessary for the caller
   * to handle cases where an entire transfer has not been completed.
   *
   * <p>Blocks until numToRead bytes have been read or timeout expires.
   * May be called with numToRead=0 to retrieve how many bytes are available.
   *
   * @param buffer array where read bytes are stored
   * @param numToRead number of bytes to read
   * @param timeout timeout in seconds (ms resolution)
   * @return Number of bytes remaining to be read
   */
  public int readAutoReceivedData(byte[] buffer, int numToRead, double timeout) {
    if (buffer.length < numToRead) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + numToRead);
    }
    return SPIJNI.spiReadAutoReceivedData(m_port, buffer, numToRead, timeout);
  }

  /**
   * Get the number of bytes dropped by the automatic SPI transfer engine due
   * to the receive buffer being full.
   *
   * @return Number of bytes dropped
   */
  public int getAutoDroppedCount() {
    return SPIJNI.spiGetAutoDroppedCount(m_port);
  }

  private static final int kAccumulateDepth = 2048;

  private static class Accumulator {
    Accumulator(int port, int xferSize, int validMask, int validValue, int dataShift,
                int dataSize, boolean isSigned, boolean bigEndian) {
      m_notifier = new Notifier(this::update);
      m_buf = ByteBuffer.allocateDirect(xferSize * kAccumulateDepth);
      m_xferSize = xferSize;
      m_validMask = validMask;
      m_validValue = validValue;
      m_dataShift = dataShift;
      m_dataMax = 1 << dataSize;
      m_dataMsbMask = 1 << (dataSize - 1);
      m_isSigned = isSigned;
      m_bigEndian = bigEndian;
      m_port = port;
    }

    void free() {
      m_notifier.stop();
    }

    final Notifier m_notifier;
    final ByteBuffer m_buf;
    final Object m_mutex = new Object();

    long m_value;
    int m_count;
    int m_lastValue;

    int m_center;
    int m_deadband;

    final int m_validMask;
    final int m_validValue;
    final int m_dataMax;        // one more than max data value
    final int m_dataMsbMask;    // data field MSB mask (for signed)
    final int m_dataShift;      // data field shift right amount, in bits
    final int m_xferSize;       // SPI transfer size, in bytes
    final boolean m_isSigned;   // is data field signed?
    final boolean m_bigEndian;  // is response big endian?
    final int m_port;

    void update() {
      synchronized (m_mutex) {
        boolean done = false;
        while (!done) {
          done = true;

          // get amount of data available
          int numToRead = SPIJNI.spiReadAutoReceivedData(m_port, m_buf, 0, 0);

          // only get whole responses
          numToRead -= numToRead % m_xferSize;
          if (numToRead > m_xferSize * kAccumulateDepth) {
            numToRead = m_xferSize * kAccumulateDepth;
            done = false;
          }
          if (numToRead == 0) {
            return;  // no samples
          }

          // read buffered data
          SPIJNI.spiReadAutoReceivedData(m_port, m_buf, numToRead, 0);

          // loop over all responses
          for (int off = 0; off < numToRead; off += m_xferSize) {
            // convert from bytes
            int resp = 0;
            if (m_bigEndian) {
              for (int i = 0; i < m_xferSize; ++i) {
                resp <<= 8;
                resp |= ((int) m_buf.get(off + i)) & 0xff;
              }
            } else {
              for (int i = m_xferSize - 1; i >= 0; --i) {
                resp <<= 8;
                resp |= ((int) m_buf.get(off + i)) & 0xff;
              }
            }

            // process response
            if ((resp & m_validMask) == m_validValue) {
              // valid sensor data; extract data field
              int data = resp >> m_dataShift;
              data &= m_dataMax - 1;
              // 2s complement conversion if signed MSB is set
              if (m_isSigned && (data & m_dataMsbMask) != 0) {
                data -= m_dataMax;
              }
              // center offset
              data -= m_center;
              // only accumulate if outside deadband
              if (data < -m_deadband || data > m_deadband) {
                m_value += data;
              }
              ++m_count;
              m_lastValue = data;
            } else {
              // no data from the sensor; just clear the last value
              m_lastValue = 0;
            }
          }
        }
      }
    }
  }

  private Accumulator m_accum = null;

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
    initAuto(xferSize * 2048);
    byte[] cmdBytes = new byte[] {0, 0, 0, 0};
    if (bigEndian) {
      for (int i = xferSize - 1; i >= 0; --i) {
        cmdBytes[i] = (byte) (cmd & 0xff);
        cmd >>= 8;
      }
    } else {
      cmdBytes[0] = (byte) (cmd & 0xff);
      cmd >>= 8;
      cmdBytes[1] = (byte) (cmd & 0xff);
      cmd >>= 8;
      cmdBytes[2] = (byte) (cmd & 0xff);
      cmd >>= 8;
      cmdBytes[3] = (byte) (cmd & 0xff);
    }
    setAutoTransmitData(cmdBytes, xferSize - 4);
    startAutoRate(period);

    m_accum = new Accumulator(m_port, xferSize, validMask, validValue, dataShift, dataSize,
                              isSigned, bigEndian);
    m_accum.m_notifier.startPeriodic(period * 1024);
  }

  /**
   * Frees the accumulator.
   */
  public void freeAccumulator() {
    if (m_accum != null) {
      m_accum.free();
      m_accum = null;
    }
    freeAuto();
  }

  /**
   * Resets the accumulator to zero.
   */
  public void resetAccumulator() {
    if (m_accum == null) {
      return;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.m_value = 0;
      m_accum.m_count = 0;
      m_accum.m_lastValue = 0;
    }
  }

  /**
   * Set the center value of the accumulator.
   *
   * <p>The center value is subtracted from each value before it is added to the accumulator. This
   * is used for the center value of devices like gyros and accelerometers to make integration work
   * and to take the device offset into account when integrating.
   */
  public void setAccumulatorCenter(int center) {
    if (m_accum == null) {
      return;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.m_center = center;
    }
  }

  /**
   * Set the accumulator's deadband.
   */
  public void setAccumulatorDeadband(int deadband) {
    if (m_accum == null) {
      return;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.m_deadband = deadband;
    }
  }

  /**
   * Read the last value read by the accumulator engine.
   */
  public int getAccumulatorLastValue() {
    if (m_accum == null) {
      return 0;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      return m_accum.m_lastValue;
    }
  }

  /**
   * Read the accumulated value.
   *
   * @return The 64-bit value accumulated since the last Reset().
   */
  public long getAccumulatorValue() {
    if (m_accum == null) {
      return 0;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      return m_accum.m_value;
    }
  }

  /**
   * Read the number of accumulated values.
   *
   * <p>Read the count of the accumulated values since the accumulator was last Reset().
   *
   * @return The number of times samples from the channel were accumulated.
   */
  public int getAccumulatorCount() {
    if (m_accum == null) {
      return 0;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      return m_accum.m_count;
    }
  }

  /**
   * Read the average of the accumulated value.
   *
   * @return The accumulated average value (value / count).
   */
  public double getAccumulatorAverage() {
    if (m_accum == null) {
      return 0;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      if (m_accum.m_count == 0) {
        return 0.0;
      }
      return ((double) m_accum.m_value) / m_accum.m_count;
    }
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
    if (m_accum == null) {
      result.value = 0;
      result.count = 0;
      return;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      result.value = m_accum.m_value;
      result.count = m_accum.m_count;
    }
  }
}
