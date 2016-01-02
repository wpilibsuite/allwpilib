/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.UnsupportedEncodingException;

import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.SerialPortJNI;

/**
 * Driver for the RS-232 serial port on the RoboRIO.
 *
 * The current implementation uses the VISA formatted I/O mode. This means that
 * all traffic goes through the formatted buffers. This allows the intermingled
 * use of print(), readString(), and the raw buffer accessors read() and
 * write().
 *
 * More information can be found in the NI-VISA User Manual here:
 * http://www.ni.com/pdf/manuals/370423a.pdf and the NI-VISA Programmer's
 * Reference Manual here: http://www.ni.com/pdf/manuals/370132c.pdf
 */
public class SerialPort {

  private byte m_port;

  public enum Port {
    kOnboard(0), kMXP(1), kUSB(2);

    private int value;

    private Port(int value) {
      this.value = value;
    }

    public int getValue() {
      return this.value;
    }
  };

  /**
   * Represents the parity to use for serial communications
   */
  public static class Parity {

    /**
     * The integer value representing this enumeration
     */
    public final int value;
    static final int kNone_val = 0;
    static final int kOdd_val = 1;
    static final int kEven_val = 2;
    static final int kMark_val = 3;
    static final int kSpace_val = 4;
    /**
     * parity: Use no parity
     */
    public static final Parity kNone = new Parity(kNone_val);
    /**
     * parity: Use odd parity
     */
    public static final Parity kOdd = new Parity(kOdd_val);
    /**
     * parity: Use even parity
     */
    public static final Parity kEven = new Parity(kEven_val);
    /**
     * parity: Use mark parity
     */
    public static final Parity kMark = new Parity(kMark_val);
    /**
     * parity: Use space parity
     */
    public static final Parity kSpace = new Parity((kSpace_val));

    private Parity(int value) {
      this.value = value;
    }
  }

  /**
   * Represents the number of stop bits to use for Serial Communication
   */
  public static class StopBits {

    /**
     * The integer value representing this enumeration
     */
    public final int value;
    static final int kOne_val = 10;
    static final int kOnePointFive_val = 15;
    static final int kTwo_val = 20;
    /**
     * stopBits: use 1
     */
    public static final StopBits kOne = new StopBits(kOne_val);
    /**
     * stopBits: use 1.5
     */
    public static final StopBits kOnePointFive = new StopBits(kOnePointFive_val);
    /**
     * stopBits: use 2
     */
    public static final StopBits kTwo = new StopBits(kTwo_val);

    private StopBits(int value) {
      this.value = value;
    }
  }

  /**
   * Represents what type of flow control to use for serial communication
   */
  public static class FlowControl {

    /**
     * The integer value representing this enumeration
     */
    public final int value;
    static final int kNone_val = 0;
    static final int kXonXoff_val = 1;
    static final int kRtsCts_val = 2;
    static final int kDtrDsr_val = 4;
    /**
     * flowControl: use none
     */
    public static final FlowControl kNone = new FlowControl(kNone_val);
    /**
     * flowcontrol: use on/off
     */
    public static final FlowControl kXonXoff = new FlowControl(kXonXoff_val);
    /**
     * flowcontrol: use rts cts
     */
    public static final FlowControl kRtsCts = new FlowControl(kRtsCts_val);
    /**
     * flowcontrol: use dts dsr
     */
    public static final FlowControl kDtrDsr = new FlowControl(kDtrDsr_val);

    private FlowControl(int value) {
      this.value = value;
    }
  }

  /**
   * Represents which type of buffer mode to use when writing to a serial port
   */
  public static class WriteBufferMode {

    /**
     * The integer value representing this enumeration
     */
    public final int value;
    static final int kFlushOnAccess_val = 1;
    static final int kFlushWhenFull_val = 2;
    /**
     * Flush on access
     */
    public static final WriteBufferMode kFlushOnAccess = new WriteBufferMode(kFlushOnAccess_val);
    /**
     * Flush when full
     */
    public static final WriteBufferMode kFlushWhenFull = new WriteBufferMode(kFlushWhenFull_val);

    private WriteBufferMode(int value) {
      this.value = value;
    }
  }

  /**
   * Create an instance of a Serial Port class.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port The Serial port to use
   * @param dataBits The number of data bits per transfer. Valid values are
   *        between 5 and 8 bits.
   * @param parity Select the type of parity checking to use.
   * @param stopBits The number of stop bits to use as defined by the enum
   *        StopBits.
   */
  public SerialPort(final int baudRate, Port port, final int dataBits, Parity parity,
      StopBits stopBits) {
    m_port = (byte) port.getValue();

    SerialPortJNI.serialInitializePort(m_port);
    SerialPortJNI.serialSetBaudRate(m_port, baudRate);
    SerialPortJNI.serialSetDataBits(m_port, (byte) dataBits);
    SerialPortJNI.serialSetParity(m_port, (byte) parity.value);
    SerialPortJNI.serialSetStopBits(m_port, (byte) stopBits.value);

    // Set the default read buffer size to 1 to return bytes immediately
    setReadBufferSize(1);

    // Set the default timeout to 5 seconds.
    setTimeout(5.0f);

    // Don't wait until the buffer is full to transmit.
    setWriteBufferMode(WriteBufferMode.kFlushOnAccess);

    disableTermination();

    UsageReporting.report(tResourceType.kResourceType_SerialPort, 0);
  }

  /**
   * Create an instance of a Serial Port class. Defaults to one stop bit.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param dataBits The number of data bits per transfer. Valid values are
   *        between 5 and 8 bits.
   * @param parity Select the type of parity checking to use.
   */
  public SerialPort(final int baudRate, Port port, final int dataBits, Parity parity) {
    this(baudRate, port, dataBits, parity, StopBits.kOne);
  }

  /**
   * Create an instance of a Serial Port class. Defaults to no parity and one
   * stop bit.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param dataBits The number of data bits per transfer. Valid values are
   *        between 5 and 8 bits.
   */
  public SerialPort(final int baudRate, Port port, final int dataBits) {
    this(baudRate, port, dataBits, Parity.kNone, StopBits.kOne);
  }

  /**
   * Create an instance of a Serial Port class. Defaults to 8 databits, no
   * parity, and one stop bit.
   *
   * @param baudRate The baud rate to configure the serial port.
   */
  public SerialPort(final int baudRate, Port port) {
    this(baudRate, port, 8, Parity.kNone, StopBits.kOne);
  }

  /**
   * Destructor.
   */
  public void free() {
    SerialPortJNI.serialClose(m_port);
  }

  /**
   * Set the type of flow control to enable on this port.
   *
   * By default, flow control is disabled.
   *$
   * @param flowControl the FlowControl value to use
   */
  public void setFlowControl(FlowControl flowControl) {
    SerialPortJNI.serialSetFlowControl(m_port, (byte) flowControl.value);
  }

  /**
   * Enable termination and specify the termination character.
   *
   * Termination is currently only implemented for receive. When the the
   * terminator is received, the read() or readString() will return fewer bytes
   * than requested, stopping after the terminator.
   *
   * @param terminator The character to use for termination.
   */
  public void enableTermination(char terminator) {
    SerialPortJNI.serialEnableTermination(m_port, terminator);
  }

  /**
   * Enable termination with the default terminator '\n'
   *
   * Termination is currently only implemented for receive. When the the
   * terminator is received, the read() or readString() will return fewer bytes
   * than requested, stopping after the terminator.
   *
   * The default terminator is '\n'
   */
  public void enableTermination() {
    this.enableTermination('\n');
  }

  /**
   * Disable termination behavior.
   */
  public void disableTermination() {
    SerialPortJNI.serialDisableTermination(m_port);
  }

  /**
   * Get the number of bytes currently available to read from the serial port.
   *
   * @return The number of bytes available to read.
   */
  public int getBytesReceived() {
    return SerialPortJNI.serialGetBytesRecieved(m_port);
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
    try {
      return new String(out, 0, out.length, "US-ASCII");
    } catch (UnsupportedEncodingException ex) {
      ex.printStackTrace();
      return new String();
    }
  }

  /**
   * Read raw bytes out of the buffer.
   *
   * @param count The maximum number of bytes to read.
   * @return An array of the read bytes
   */
  public byte[] read(final int count) {
    ByteBuffer dataReceivedBuffer = ByteBuffer.allocateDirect(count);
    int gotten = SerialPortJNI.serialRead(m_port, dataReceivedBuffer, count);
    byte[] retVal = new byte[gotten];
    dataReceivedBuffer.get(retVal);
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
    ByteBuffer dataToSendBuffer = ByteBuffer.allocateDirect(count);
    dataToSendBuffer.put(buffer, 0, count);
    return SerialPortJNI.serialWrite(m_port, dataToSendBuffer, count);
  }

  /**
   * Write a string to the serial port
   *
   * @param data The string to write to the serial port.
   * @return The number of bytes actually written into the port.
   */
  public int writeString(String data) {
    return write(data.getBytes(), data.length());
  }

  /**
   * Configure the timeout of the serial port.
   *
   * This defines the timeout for transactions with the hardware. It will affect
   * reads if less bytes are available than the read buffer size (defaults to 1)
   * and very large writes.
   *
   * @param timeout The number of seconds to to wait for I/O.
   */
  public void setTimeout(double timeout) {
    SerialPortJNI.serialSetTimeout(m_port, (float) timeout);
  }

  /**
   * Specify the size of the input buffer.
   *
   * Specify the amount of data that can be stored before data from the device
   * is returned to Read. If you want data that is received to be returned
   * immediately, set this to 1.
   *
   * It the buffer is not filled before the read timeout expires, all data that
   * has been received so far will be returned.
   *
   * @param size The read buffer size.
   */
  public void setReadBufferSize(int size) {
    SerialPortJNI.serialSetReadBufferSize(m_port, size);
  }

  /**
   * Specify the size of the output buffer.
   *
   * Specify the amount of data that can be stored before being transmitted to
   * the device.
   *
   * @param size The write buffer size.
   */
  public void setWriteBufferSize(int size) {
    SerialPortJNI.serialSetWriteBufferSize(m_port, size);
  }

  /**
   * Specify the flushing behavior of the output buffer.
   *
   * When set to kFlushOnAccess, data is synchronously written to the serial
   * port after each call to either print() or write().
   *
   * When set to kFlushWhenFull, data will only be written to the serial port
   * when the buffer is full or when flush() is called.
   *
   * @param mode The write buffer mode.
   */
  public void setWriteBufferMode(WriteBufferMode mode) {
    SerialPortJNI.serialSetWriteMode(m_port, (byte) mode.value);
  }

  /**
   * Force the output buffer to be written to the port.
   *
   * This is used when setWriteBufferMode() is set to kFlushWhenFull to force a
   * flush before the buffer is full.
   */
  public void flush() {
    SerialPortJNI.serialFlush(m_port);
  }

  /**
   * Reset the serial port driver to a known state.
   *
   * Empty the transmit and receive buffers in the device and formatted I/O.
   */
  public void reset() {
    SerialPortJNI.serialClear(m_port);
  }
}
