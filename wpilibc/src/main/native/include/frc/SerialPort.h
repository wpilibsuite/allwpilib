// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <hal/SerialPort.h>
#include <hal/Types.h>
#include <units/time.h>

namespace frc {

/**
 * Driver for the RS-232 serial port on the roboRIO.
 *
 * The current implementation uses the VISA formatted I/O mode.  This means that
 * all traffic goes through the formatted buffers.  This allows the intermingled
 * use of Printf(), Scanf(), and the raw buffer accessors Read() and Write().
 *
 * More information can be found in the NI-VISA User Manual here:
 *   http://www.ni.com/pdf/manuals/370423a.pdf
 * and the NI-VISA Programmer's Reference Manual here:
 *   http://www.ni.com/pdf/manuals/370132c.pdf
 */
class SerialPort {
 public:
  /**
   * Serial port.
   */
  enum Port {
    /// Onboard serial port on the roboRIO.
    ONBOARD = 0,
    /// MXP (roboRIO MXP) serial port.
    MXP = 1,
    /// USB serial port (same as USB1).
    USB = 2,
    /// USB serial port 1.
    USB1 = 2,
    /// USB serial port 2.
    USB2 = 3
  };

  /**
   * Represents the parity to use for serial communications.
   */
  enum Parity {
    /// No parity.
    PARITY_NONE = 0,
    /// Odd parity.
    PARITY_ODD = 1,
    /// Even parity.
    PARITY_EVEN = 2,
    /// Parity bit always on.
    PARITY_MARK = 3,
    /// Parity bit always off.
    PARITY_SPACE = 4
  };

  /**
   * Represents the number of stop bits to use for Serial Communication.
   */
  enum StopBits {
    /// One stop bit.
    STOP_BITS_ONE = 10,
    /// One and a half stop bits.
    STOP_BITS_ONE_POINT_FIVE = 15,
    /// Two stop bits.
    STOP_BITS_TWO = 20
  };

  /**
   * Represents what type of flow control to use for serial communication.
   */
  enum FlowControl {
    /// No flow control.
    FLOW_CONTROL_NONE = 0,
    /// XON/XOFF flow control.
    FLOW_CONTROL_XONXOFF = 1,
    /// RTS/CTS flow control.
    FLOW_CONTROL_RTSCTS = 2,
    /// DTS/DSR flow control.
    FLOW_CONTROL_DTRDSR = 4
  };

  /**
   * Represents which type of buffer mode to use when writing to a serial port.
   */
  enum WriteBufferMode {
    /// Flush the buffer on each access.
    FLUSH_ON_ACCESS = 1,
    /// Flush the buffer when it is full.
    FLUSH_WHEN_FULL = 2
  };

  /**
   * Create an instance of a Serial Port class.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port     The physical port to use
   * @param dataBits The number of data bits per transfer.  Valid values are
   *                 between 5 and 8 bits.
   * @param parity   Select the type of parity checking to use.
   * @param stopBits The number of stop bits to use as defined by the enum
   *                 StopBits.
   */
  explicit SerialPort(int baudRate, Port port = ONBOARD, int dataBits = 8,
                      Parity parity = PARITY_NONE,
                      StopBits stopBits = STOP_BITS_ONE);

  /**
   * Create an instance of a Serial Port class.
   *
   * Prefer to use the constructor that doesn't take a port name, but in some
   * cases the automatic detection might not work correctly.
   *
   * @param baudRate The baud rate to configure the serial port.
   * @param port     The physical port to use
   * @param portName The direct port name to use
   * @param dataBits The number of data bits per transfer.  Valid values are
   *                 between 5 and 8 bits.
   * @param parity   Select the type of parity checking to use.
   * @param stopBits The number of stop bits to use as defined by the enum
   *                 StopBits.
   */
  SerialPort(int baudRate, std::string_view portName, Port port = ONBOARD,
             int dataBits = 8, Parity parity = PARITY_NONE,
             StopBits stopBits = STOP_BITS_ONE);

  SerialPort(SerialPort&& rhs) = default;
  SerialPort& operator=(SerialPort&& rhs) = default;

  /**
   * Set the type of flow control to enable on this port.
   *
   * By default, flow control is disabled.
   */
  void SetFlowControl(FlowControl flowControl);

  /**
   * Enable termination and specify the termination character.
   *
   * Termination is currently only implemented for receive.
   * When the the terminator is received, the Read() or Scanf() will return
   * fewer bytes than requested, stopping after the terminator.
   *
   * @param terminator The character to use for termination.
   */
  void EnableTermination(char terminator = '\n');

  /**
   * Disable termination behavior.
   */
  void DisableTermination();

  /**
   * Get the number of bytes currently available to read from the serial port.
   *
   * @return The number of bytes available to read
   */
  int GetBytesReceived();

  /**
   * Read raw bytes out of the buffer.
   *
   * @param buffer Pointer to the buffer to store the bytes in.
   * @param count  The maximum number of bytes to read.
   * @return The number of bytes actually read into the buffer.
   */
  int Read(char* buffer, int count);

  /**
   * Write raw bytes to the buffer.
   *
   * @param buffer Pointer to the buffer to read the bytes from.
   * @param count  The maximum number of bytes to write.
   * @return The number of bytes actually written into the port.
   */
  int Write(const char* buffer, int count);

  /**
   * Write raw bytes to the buffer.
   *
   * Use Write({data, len}) to get a buffer that is shorter than the length of
   * the string.
   *
   * @param buffer the buffer to read the bytes from.
   * @return The number of bytes actually written into the port.
   */
  int Write(std::string_view buffer);

  /**
   * Configure the timeout of the serial port.
   *
   * This defines the timeout for transactions with the hardware.
   * It will affect reads and very large writes.
   *
   * @param timeout The time to wait for I/O.
   */
  void SetTimeout(units::second_t timeout);

  /**
   * Specify the size of the input buffer.
   *
   * Specify the amount of data that can be stored before data
   * from the device is returned to Read or Scanf.  If you want
   * data that is received to be returned immediately, set this to 1.
   *
   * It the buffer is not filled before the read timeout expires, all
   * data that has been received so far will be returned.
   *
   * @param size The read buffer size.
   */
  void SetReadBufferSize(int size);

  /**
   * Specify the size of the output buffer.
   *
   * Specify the amount of data that can be stored before being
   * transmitted to the device.
   *
   * @param size The write buffer size.
   */
  void SetWriteBufferSize(int size);

  /**
   * Specify the flushing behavior of the output buffer.
   *
   * When set to FLUSH_ON_ACCESS, data is synchronously written to the serial
   * port after each call to either Printf() or Write().
   *
   * When set to FLUSH_WHEN_FULL, data will only be written to the serial port
   * when the buffer is full or when Flush() is called.
   *
   * @param mode The write buffer mode.
   */
  void SetWriteBufferMode(WriteBufferMode mode);

  /**
   * Force the output buffer to be written to the port.
   *
   * This is used when SetWriteBufferMode() is set to FLUSH_WHEN_FULL to force a
   * flush before the buffer is full.
   */
  void Flush();

  /**
   * Reset the serial port driver to a known state.
   *
   * Empty the transmit and receive buffers in the device and formatted I/O.
   */
  void Reset();

 private:
  hal::Handle<HAL_SerialPortHandle, HAL_CloseSerial> m_portHandle;
};

}  // namespace frc
