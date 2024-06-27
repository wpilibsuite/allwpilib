// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AccumulatorResult;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SPIJNI;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

/** Represents an SPI bus port. */
public class SPI implements AutoCloseable {
  /** SPI port. */
  public enum Port {
    /** Onboard SPI bus port CS0. */
    kOnboardCS0(SPIJNI.ONBOARD_CS0_PORT),
    /** Onboard SPI bus port CS1. */
    kOnboardCS1(SPIJNI.ONBOARD_CS1_PORT),
    /** Onboard SPI bus port CS2. */
    kOnboardCS2(SPIJNI.ONBOARD_CS2_PORT),
    /** Onboard SPI bus port CS3. */
    kOnboardCS3(SPIJNI.ONBOARD_CS3_PORT),
    /** MXP (roboRIO MXP) SPI bus port. */
    kMXP(SPIJNI.MXP_PORT);

    /** SPI port value. */
    public final int value;

    Port(int value) {
      this.value = value;
    }
  }

  /** SPI mode. */
  public enum Mode {
    /** Clock idle low, data sampled on rising edge. */
    kMode0(SPIJNI.SPI_MODE0),
    /** Clock idle low, data sampled on falling edge. */
    kMode1(SPIJNI.SPI_MODE1),
    /** Clock idle high, data sampled on falling edge. */
    kMode2(SPIJNI.SPI_MODE2),
    /** Clock idle high, data sampled on rising edge. */
    kMode3(SPIJNI.SPI_MODE3);

    /** SPI mode value. */
    public final int value;

    Mode(int value) {
      this.value = value;
    }
  }

  private final int m_port;

  /**
   * Constructor.
   *
   * @param port the physical SPI port
   */
  public SPI(Port port) {
    m_port = port.value;

    SPIJNI.spiInitialize(m_port);

    SPIJNI.spiSetMode(m_port, 0);

    HAL.report(tResourceType.kResourceType_SPI, port.value + 1);
  }

  /**
   * Returns the SPI port value.
   *
   * @return SPI port value.
   */
  public int getPort() {
    return m_port;
  }

  @Override
  public void close() {
    if (m_accum != null) {
      m_accum.close();
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
   * Sets the mode for the SPI device.
   *
   * <p>Mode 0 is Clock idle low, data sampled on rising edge.
   *
   * <p>Mode 1 is Clock idle low, data sampled on falling edge.
   *
   * <p>Mode 2 is Clock idle high, data sampled on falling edge.
   *
   * <p>Mode 3 is Clock idle high, data sampled on rising edge.
   *
   * @param mode The mode to set.
   */
  public final void setMode(Mode mode) {
    SPIJNI.spiSetMode(m_port, mode.value & 0x3);
  }

  /** Configure the chip select line to be active high. */
  public final void setChipSelectActiveHigh() {
    SPIJNI.spiSetChipSelectActiveHigh(m_port);
  }

  /** Configure the chip select line to be active low. */
  public final void setChipSelectActiveLow() {
    SPIJNI.spiSetChipSelectActiveLow(m_port);
  }

  /**
   * Write data to the peripheral device. Blocks until there is space in the output FIFO.
   *
   * <p>If not running in output only mode, also saves the data received on the CIPO input during
   * the transfer into the receive FIFO.
   *
   * @param dataToSend The buffer containing the data to send.
   * @param size The number of bytes to send.
   * @return Number of bytes written or -1 on error.
   */
  public int write(byte[] dataToSend, int size) {
    if (dataToSend.length < size) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + size);
    }
    return SPIJNI.spiWriteB(m_port, dataToSend, (byte) size);
  }

  /**
   * Write data to the peripheral device. Blocks until there is space in the output FIFO.
   *
   * <p>If not running in output only mode, also saves the data received on the CIPO input during
   * the transfer into the receive FIFO.
   *
   * @param dataToSend The buffer containing the data to send.
   * @param size The number of bytes to send.
   * @return Number of bytes written or -1 on error.
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
   *     transfer. If false, this function assumes that data is already in the receive FIFO from a
   *     previous write.
   * @param dataReceived Buffer in which to store bytes read.
   * @param size Number of bytes to read.
   * @return Number of bytes read or -1 on error.
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
   * @param initiate If true, this function pushes "0" into the transmit buffer and initiates a
   *     transfer. If false, this function assumes that data is already in the receive FIFO from a
   *     previous write.
   * @param dataReceived The buffer to be filled with the received data.
   * @param size The length of the transaction, in bytes
   * @return Number of bytes read or -1 on error.
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
   * @param dataToSend The data to be written out to the device
   * @param dataReceived Buffer to receive data from the device
   * @param size The length of the transaction, in bytes
   * @return TODO
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
   * @param dataToSend The data to be written out to the device.
   * @param dataReceived Buffer to receive data from the device.
   * @param size The length of the transaction, in bytes
   * @return TODO
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
   * <p>Only a single engine is available, and use of it blocks use of all other chip select usage
   * on the same physical SPI port while it is running.
   *
   * @param bufferSize buffer size in bytes
   */
  public void initAuto(int bufferSize) {
    SPIJNI.spiInitAuto(m_port, bufferSize);
  }

  /** Frees the automatic SPI transfer engine. */
  public void freeAuto() {
    SPIJNI.spiFreeAuto(m_port);
  }

  /**
   * Set the data to be transmitted by the engine.
   *
   * <p>Up to 16 bytes are configurable, and may be followed by up to 127 zero bytes.
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
   * <p>{@link #initAuto(int)} and {@link #setAutoTransmitData(byte[], int)} must be called before
   * calling this function.
   *
   * @param period period between transfers, in seconds (us resolution)
   */
  public void startAutoRate(double period) {
    SPIJNI.spiStartAutoRate(m_port, period);
  }

  /**
   * Start running the automatic SPI transfer engine when a trigger occurs.
   *
   * <p>{@link #initAuto(int)} and {@link #setAutoTransmitData(byte[], int)} must be called before
   * calling this function.
   *
   * @param source digital source for the trigger (may be an analog trigger)
   * @param rising trigger on the rising edge
   * @param falling trigger on the falling edge
   */
  public void startAutoTrigger(DigitalSource source, boolean rising, boolean falling) {
    SPIJNI.spiStartAutoTrigger(
        m_port,
        source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting(),
        rising,
        falling);
  }

  /** Stop running the automatic SPI transfer engine. */
  public void stopAuto() {
    SPIJNI.spiStopAuto(m_port);
  }

  /** Force the engine to make a single transfer. */
  public void forceAutoRead() {
    SPIJNI.spiForceAutoRead(m_port);
  }

  /**
   * Read data that has been transferred by the automatic SPI transfer engine.
   *
   * <p>Transfers may be made a byte at a time, so it's necessary for the caller to handle cases
   * where an entire transfer has not been completed.
   *
   * <p>Each received data sequence consists of a timestamp followed by the received data bytes, one
   * byte per word (in the least significant byte). The length of each received data sequence is the
   * same as the combined size of the data and zeroSize set in setAutoTransmitData().
   *
   * <p>Blocks until numToRead words have been read or timeout expires. May be called with
   * numToRead=0 to retrieve how many words are available.
   *
   * @param buffer buffer where read words are stored
   * @param numToRead number of words to read
   * @param timeout timeout in seconds (ms resolution)
   * @return Number of words remaining to be read
   */
  public int readAutoReceivedData(ByteBuffer buffer, int numToRead, double timeout) {
    if (!buffer.isDirect()) {
      throw new IllegalArgumentException("must be a direct buffer");
    }
    if (buffer.capacity() < numToRead * 4) {
      throw new IllegalArgumentException(
          "buffer is too small, must be at least " + (numToRead * 4));
    }
    return SPIJNI.spiReadAutoReceivedData(m_port, buffer, numToRead, timeout);
  }

  /**
   * Read data that has been transferred by the automatic SPI transfer engine.
   *
   * <p>Transfers may be made a byte at a time, so it's necessary for the caller to handle cases
   * where an entire transfer has not been completed.
   *
   * <p>Each received data sequence consists of a timestamp followed by the received data bytes, one
   * byte per word (in the least significant byte). The length of each received data sequence is the
   * same as the combined size of the data and zeroSize set in setAutoTransmitData().
   *
   * <p>Blocks until numToRead words have been read or timeout expires. May be called with
   * numToRead=0 to retrieve how many words are available.
   *
   * @param buffer array where read words are stored
   * @param numToRead number of words to read
   * @param timeout timeout in seconds (ms resolution)
   * @return Number of words remaining to be read
   */
  public int readAutoReceivedData(int[] buffer, int numToRead, double timeout) {
    if (buffer.length < numToRead) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + numToRead);
    }
    return SPIJNI.spiReadAutoReceivedData(m_port, buffer, numToRead, timeout);
  }

  /**
   * Get the number of bytes dropped by the automatic SPI transfer engine due to the receive buffer
   * being full.
   *
   * @return Number of bytes dropped
   */
  public int getAutoDroppedCount() {
    return SPIJNI.spiGetAutoDroppedCount(m_port);
  }

  /**
   * Configure the Auto SPI Stall time between reads.
   *
   * @param csToSclkTicks the number of ticks to wait before asserting the cs pin
   * @param stallTicks the number of ticks to stall for
   * @param pow2BytesPerRead the number of bytes to read before stalling
   */
  public void configureAutoStall(int csToSclkTicks, int stallTicks, int pow2BytesPerRead) {
    SPIJNI.spiConfigureAutoStall(m_port, csToSclkTicks, stallTicks, pow2BytesPerRead);
  }

  private static final int kAccumulateDepth = 2048;

  private static class Accumulator implements AutoCloseable {
    Accumulator(
        int port,
        int xferSize,
        int validMask,
        int validValue,
        int dataShift,
        int dataSize,
        boolean isSigned,
        boolean bigEndian) {
      m_notifier = new Notifier(this::update);
      m_buf =
          ByteBuffer.allocateDirect((xferSize + 1) * kAccumulateDepth * 4)
              .order(ByteOrder.nativeOrder());
      m_intBuf = m_buf.asIntBuffer();
      m_xferSize = xferSize + 1; // +1 for timestamp
      m_validMask = validMask;
      m_validValue = validValue;
      m_dataShift = dataShift;
      m_dataMax = 1 << dataSize;
      m_dataMsbMask = 1 << (dataSize - 1);
      m_isSigned = isSigned;
      m_bigEndian = bigEndian;
      m_port = port;
    }

    @Override
    public void close() {
      m_notifier.close();
    }

    final Notifier m_notifier;
    final ByteBuffer m_buf;
    final IntBuffer m_intBuf;
    final Object m_mutex = new Object();

    long m_value;
    int m_count;
    int m_lastValue;
    long m_lastTimestamp;
    double m_integratedValue;

    int m_center;
    int m_deadband;
    double m_integratedCenter;

    final int m_validMask;
    final int m_validValue;
    final int m_dataMax; // one more than max data value
    final int m_dataMsbMask; // data field MSB mask (for signed)
    final int m_dataShift; // data field shift right amount, in bits
    final int m_xferSize; // SPI transfer size, in bytes
    final boolean m_isSigned; // is data field signed?
    final boolean m_bigEndian; // is response big endian?
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
            return; // no samples
          }

          // read buffered data
          SPIJNI.spiReadAutoReceivedData(m_port, m_buf, numToRead, 0);

          // loop over all responses
          for (int off = 0; off < numToRead; off += m_xferSize) {
            // get timestamp from first word
            long timestamp = m_intBuf.get(off) & 0xffffffffL;

            // convert from bytes
            int resp = 0;
            if (m_bigEndian) {
              for (int i = 1; i < m_xferSize; ++i) {
                resp <<= 8;
                resp |= m_intBuf.get(off + i) & 0xff;
              }
            } else {
              for (int i = m_xferSize - 1; i >= 1; --i) {
                resp <<= 8;
                resp |= m_intBuf.get(off + i) & 0xff;
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
              int dataNoCenter = data;
              data -= m_center;
              // only accumulate if outside deadband
              if (data < -m_deadband || data > m_deadband) {
                m_value += data;
                if (m_count != 0) {
                  // timestamps use the 1us FPGA clock; also handle rollover
                  if (timestamp >= m_lastTimestamp) {
                    m_integratedValue +=
                        dataNoCenter * (timestamp - m_lastTimestamp) * 1e-6 - m_integratedCenter;
                  } else {
                    m_integratedValue +=
                        dataNoCenter * ((1L << 32) - m_lastTimestamp + timestamp) * 1e-6
                            - m_integratedCenter;
                  }
                }
              }
              ++m_count;
              m_lastValue = data;
            } else {
              // no data from the sensor; just clear the last value
              m_lastValue = 0;
            }
            m_lastTimestamp = timestamp;
          }
        }
      }
    }
  }

  private Accumulator m_accum;

  /**
   * Initialize the accumulator.
   *
   * @param period Time between reads
   * @param cmd SPI command to send to request data
   * @param xferSize SPI transfer size, in bytes
   * @param validMask Mask to apply to received data for validity checking
   * @param validValue After validMask is applied, required matching value for validity checking
   * @param dataShift Bit shift to apply to received data to get actual data value
   * @param dataSize Size (in bits) of data field
   * @param isSigned Is data field signed?
   * @param bigEndian Is device big endian?
   */
  public void initAccumulator(
      double period,
      int cmd,
      int xferSize,
      int validMask,
      int validValue,
      int dataShift,
      int dataSize,
      boolean isSigned,
      boolean bigEndian) {
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

    m_accum =
        new Accumulator(
            m_port, xferSize, validMask, validValue, dataShift, dataSize, isSigned, bigEndian);
    m_accum.m_notifier.startPeriodic(period * 1024);
  }

  /** Frees the accumulator. */
  public void freeAccumulator() {
    if (m_accum != null) {
      m_accum.close();
      m_accum = null;
    }
    freeAuto();
  }

  /** Resets the accumulator to zero. */
  public void resetAccumulator() {
    if (m_accum == null) {
      return;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.m_value = 0;
      m_accum.m_count = 0;
      m_accum.m_lastValue = 0;
      m_accum.m_lastTimestamp = 0;
      m_accum.m_integratedValue = 0;
    }
  }

  /**
   * Set the center value of the accumulator.
   *
   * <p>The center value is subtracted from each value before it is added to the accumulator. This
   * is used for the center value of devices like gyros and accelerometers to make integration work
   * and to take the device offset into account when integrating.
   *
   * @param center The accumulator's center value.
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
   *
   * @param deadband The accumulator's deadband.
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
   *
   * @return The last value read by the accumulator engine.
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

  /**
   * Set the center value of the accumulator integrator.
   *
   * <p>The center value is subtracted from each value*dt before it is added to the integrated
   * value. This is used for the center value of devices like gyros and accelerometers to take the
   * device offset into account when integrating.
   *
   * @param center The accumulator integrator's center value.
   */
  public void setAccumulatorIntegratedCenter(double center) {
    if (m_accum == null) {
      return;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.m_integratedCenter = center;
    }
  }

  /**
   * Read the integrated value. This is the sum of (each value * time between values).
   *
   * @return The integrated value accumulated since the last Reset().
   */
  public double getAccumulatorIntegratedValue() {
    if (m_accum == null) {
      return 0;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      return m_accum.m_integratedValue;
    }
  }

  /**
   * Read the average of the integrated value. This is the sum of (each value times the time between
   * values), divided by the count.
   *
   * @return The average of the integrated value accumulated since the last Reset().
   */
  public double getAccumulatorIntegratedAverage() {
    if (m_accum == null) {
      return 0;
    }
    synchronized (m_accum.m_mutex) {
      m_accum.update();
      if (m_accum.m_count <= 1) {
        return 0.0;
      }
      // count-1 due to not integrating the first value received
      return m_accum.m_integratedValue / (m_accum.m_count - 1);
    }
  }
}
