// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SerialPortJNI;
import java.nio.charset.StandardCharsets;

/** Driver for the serial ports (USB, MXP, Onboard) on the roboRIO. */
public class SerialPort implements AutoCloseable {
  private int m_portHandle;

  /** Serial port. */
  public enum Port {
    /** Onboard serial port on the roboRIO. */
    kOnboard(0),
    /** MXP (roboRIO MXP) serial port. */
    kMXP(1),
    /** USB serial port (same as kUSB1). */
    kUSB(2),
    /** USB serial port 1. */
    kUSB1(2),
    /** USB serial port 2. */
    kUSB2(3);

    /** Port value. */
    public final int value;

    Port(int value) {
      this.value = value;
    }
  }

  /** Represents the parity to use for serial communications. */
  public enum Parity {
    /** No parity. */
    kNone(0),
    /** Odd parity. */
    kOdd(1),
    /** Even parity. */
    kEven(2),
    /** Parity bit always on. */
    kMark(3),
    /** Parity bit always off. */
    kSpace(4);

    /** Parity value. */
    public final int value;

    Parity(int value) {
      this.value = value;
    }
  }

  /** Represents the number of stop bits to use for Serial Communication. */
  public enum StopBits {
    /** One stop bit. */
    kOne(10),
    /** One and a half stop bits. */
    kOnePointFive(15),
    /** Two stop bits. */
    kTwo(20);

    /** StopBits value. */
    public final int value;

    StopBits(int value) {
      this.value = value;
    }
  }

  /** Represents what type of flow control to use for serial communication. */
  public enum FlowControl {
    /** No flow control. */
    kNone(0),
    /** XON/XOFF flow control. */
    kXonXoff(1),
    /** RTS/CTS flow control. */
    kRtsCts(2),
    /** DTS/DSR flow control. */
    kDtsDsr(4);

    /** FlowControl value. */
    public final int value;

    FlowControl(int value) {
      this.value = value;
    }
  }

  /** Represents which type of buffer mode to use when writing to a serial port. */
  public enum WriteBufferMode {
    /** Flush the buffer on each access. */
    kFlushOnAccess(1),
    /** Flush the buffer when it is full. */
    kFlushWhenFull(2);

    /** WriteBufferMode value. */
    public final int value;

    WriteBufferMode(int value) {
      this.value = value;
    }
  }

  /**
   * Create an instance of a Serial Port class.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port The Serial port to use
   * @param dataBits The number of data bits per transfer. Valid values are between 5 and 8 bits.
   * @param parity Select the type of parity checking to use.
   * @param stopBits The number of stop bits to use as defined by the enum StopBits.
   */
  public SerialPort(
      final int baudRate, Port port, final int dataBits, Parity parity, StopBits stopBits) {
    m_portHandle = SerialPortJNI.serialInitializePort((byte) port.value);
    SerialPortJNI.serialSetBaudRate(m_portHandle, baudRate);
    SerialPortJNI.serialSetDataBits(m_portHandle, (byte) dataBits);
    SerialPortJNI.serialSetParity(m_portHandle, (byte) parity.value);
    SerialPortJNI.serialSetStopBits(m_portHandle, (byte) stopBits.value);

    // Set the default read buffer size to 1 to return bytes immediately
    setReadBufferSize(1);

    // Set the default timeout to 5 seconds.
    setTimeout(5.0);

    // Don't wait until the buffer is full to transmit.
    setWriteBufferMode(WriteBufferMode.kFlushOnAccess);

    disableTermination();

    HAL.report(tResourceType.kResourceType_SerialPort, port.value + 1);
  }

  /**
   * Create an instance of a Serial Port class. Defaults to one stop bit.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port The serial port to use.
   * @param dataBits The number of data bits per transfer. Valid values are between 5 and 8 bits.
   * @param parity Select the type of parity checking to use.
   */
  public SerialPort(final int baudRate, Port port, final int dataBits, Parity parity) {
    this(baudRate, port, dataBits, parity, StopBits.kOne);
  }

  /**
   * Create an instance of a Serial Port class. Defaults to no parity and one stop bit.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port The serial port to use.
   * @param dataBits The number of data bits per transfer. Valid values are between 5 and 8 bits.
   */
  public SerialPort(final int baudRate, Port port, final int dataBits) {
    this(baudRate, port, dataBits, Parity.kNone, StopBits.kOne);
  }

  /**
   * Create an instance of a Serial Port class. Defaults to 8 databits, no parity, and one stop bit.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port The serial port to use.
   */
  public SerialPort(final int baudRate, Port port) {
    this(baudRate, port, 8, Parity.kNone, StopBits.kOne);
  }

  @Override
  public void close() {
    SerialPortJNI.serialClose(m_portHandle);
  }

  /**
   * Set the type of flow control to enable on this port.
   *
   * <p>By default, flow control is disabled.
   *
   * @param flowControl the FlowControl m_value to use
   */
  public void setFlowControl(FlowControl flowControl) {
    SerialPortJNI.serialSetFlowControl(m_portHandle, (byte) flowControl.value);
  }

  /**
   * Enable termination and specify the termination character.
   *
   * <p>Termination is currently only implemented for receive. When the terminator is received, the
   * read() or readString() will return fewer bytes than requested, stopping after the terminator.
   *
   * @param terminator The character to use for termination.
   */
  public void enableTermination(char terminator) {
    SerialPortJNI.serialEnableTermination(m_portHandle, terminator);
  }

  /**
   * Enable termination with the default terminator '\n'
   *
   * <p>Termination is currently only implemented for receive. When the terminator is received, the
   * read() or readString() will return fewer bytes than requested, stopping after the terminator.
   *
   * <p>The default terminator is '\n'
   */
  public void enableTermination() {
    enableTermination('\n');
  }

  /** Disable termination behavior. */
  public final void disableTermination() {
    SerialPortJNI.serialDisableTermination(m_portHandle);
  }

  /**
   * Get the number of bytes currently available to read from the serial port.
   *
   * @return The number of bytes available to read.
   */
  public int getBytesReceived() {
    return SerialPortJNI.serialGetBytesReceived(m_portHandle);
  }

  /**
   * Read a string out of the buffer. Reads the entire contents of the buffer
   *
   * @return The read string
   */
  public String readString() {
    return readString(getBytesReceived());
  }

  /**
   * Read a string out of the buffer. Reads the entire contents of the buffer
   *
   * @param count the number of characters to read into the string
   * @return The read string
   */
  public String readString(int count) {
    byte[] out = read(count);
    return new String(out, StandardCharsets.US_ASCII);
  }

  /**
   * Read raw bytes out of the buffer.
   *
   * @param count The maximum number of bytes to read.
   * @return An array of the read bytes
   */
  public byte[] read(final int count) {
    byte[] dataReceivedBuffer = new byte[count];
    int gotten = SerialPortJNI.serialRead(m_portHandle, dataReceivedBuffer, count);
    if (gotten == count) {
      return dataReceivedBuffer;
    }
    byte[] retVal = new byte[gotten];
    System.arraycopy(dataReceivedBuffer, 0, retVal, 0, gotten);
    return retVal;
  }

  /**
   * Write raw bytes to the serial port.
   *
   * @param buffer The buffer of bytes to write.
   * @param count The maximum number of bytes to write.
   * @return The number of bytes actually written into the port.
   */
  public int write(byte[] buffer, int count) {
    if (buffer.length < count) {
      throw new IllegalArgumentException("buffer is too small, must be at least " + count);
    }
    return SerialPortJNI.serialWrite(m_portHandle, buffer, count);
  }

  /**
   * Write a string to the serial port.
   *
   * @param data The string to write to the serial port.
   * @return The number of bytes actually written into the port.
   */
  public int writeString(String data) {
    return write(data.getBytes(StandardCharsets.UTF_8), data.length());
  }

  /**
   * Configure the timeout of the serial m_port.
   *
   * <p>This defines the timeout for transactions with the hardware. It will affect reads if less
   * bytes are available than the read buffer size (defaults to 1) and very large writes.
   *
   * @param timeout The number of seconds to wait for I/O.
   */
  public final void setTimeout(double timeout) {
    SerialPortJNI.serialSetTimeout(m_portHandle, timeout);
  }

  /**
   * Specify the size of the input buffer.
   *
   * <p>Specify the amount of data that can be stored before data from the device is returned to
   * Read. If you want data that is received to be returned immediately, set this to 1.
   *
   * <p>It the buffer is not filled before the read timeout expires, all data that has been received
   * so far will be returned.
   *
   * @param size The read buffer size.
   */
  public final void setReadBufferSize(int size) {
    SerialPortJNI.serialSetReadBufferSize(m_portHandle, size);
  }

  /**
   * Specify the size of the output buffer.
   *
   * <p>Specify the amount of data that can be stored before being transmitted to the device.
   *
   * @param size The write buffer size.
   */
  public void setWriteBufferSize(int size) {
    SerialPortJNI.serialSetWriteBufferSize(m_portHandle, size);
  }

  /**
   * Specify the flushing behavior of the output buffer.
   *
   * <p>When set to kFlushOnAccess, data is synchronously written to the serial port after each call
   * to either print() or write().
   *
   * <p>When set to kFlushWhenFull, data will only be written to the serial port when the buffer is
   * full or when flush() is called.
   *
   * @param mode The write buffer mode.
   */
  public final void setWriteBufferMode(WriteBufferMode mode) {
    SerialPortJNI.serialSetWriteMode(m_portHandle, (byte) mode.value);
  }

  /**
   * Force the output buffer to be written to the port.
   *
   * <p>This is used when setWriteBufferMode() is set to kFlushWhenFull to force a flush before the
   * buffer is full.
   */
  public void flush() {
    SerialPortJNI.serialFlush(m_portHandle);
  }

  /**
   * Reset the serial port driver to a known state.
   *
   * <p>Empty the transmit and receive buffers in the device and formatted I/O.
   */
  public void reset() {
    SerialPortJNI.serialClear(m_portHandle);
  }
}
