/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <hal/Types.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/deprecated.h>

#include "frc/ErrorBase.h"

namespace frc {

/**
 * Driver for the RS-232 serial port on the roboRIO.
 *
 * The current implementation uses the VISA formatted I/O mode.  This means that
 * all traffic goes through the fomatted buffers.  This allows the intermingled
 * use of Printf(), Scanf(), and the raw buffer accessors Read() and Write().
 *
 * More information can be found in the NI-VISA User Manual here:
 *   http://www.ni.com/pdf/manuals/370423a.pdf
 * and the NI-VISA Programmer's Reference Manual here:
 *   http://www.ni.com/pdf/manuals/370132c.pdf
 */
class SerialPort : public ErrorBase {
 public:
  enum Parity {
    kParity_None = 0,
    kParity_Odd = 1,
    kParity_Even = 2,
    kParity_Mark = 3,
    kParity_Space = 4
  };

  enum StopBits {
    kStopBits_One = 10,
    kStopBits_OnePointFive = 15,
    kStopBits_Two = 20
  };

  enum FlowControl {
    kFlowControl_None = 0,
    kFlowControl_XonXoff = 1,
    kFlowControl_RtsCts = 2,
    kFlowControl_DtrDsr = 4
  };

  enum WriteBufferMode { kFlushOnAccess = 1, kFlushWhenFull = 2 };

  enum Port { kOnboard = 0, kMXP = 1, kUSB = 2, kUSB1 = 2, kUSB2 = 3 };

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
  SerialPort(int baudRate, Port port = kOnboard, int dataBits = 8,
             Parity parity = kParity_None, StopBits stopBits = kStopBits_One);

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
  SerialPort(int baudRate, const wpi::Twine& portName, Port port = kOnboard,
             int dataBits = 8, Parity parity = kParity_None,
             StopBits stopBits = kStopBits_One);

  ~SerialPort();

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
   * When the the terminator is recieved, the Read() or Scanf() will return
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
   * @param buffer StringRef to the buffer to read the bytes from.
   * @return The number of bytes actually written into the port.
   */
  int Write(wpi::StringRef buffer);

  /**
   * Configure the timeout of the serial port.
   *
   * This defines the timeout for transactions with the hardware.
   * It will affect reads and very large writes.
   *
   * @param timeout The number of seconds to to wait for I/O.
   */
  void SetTimeout(double timeout);

  /**
   * Specify the size of the input buffer.
   *
   * Specify the amount of data that can be stored before data
   * from the device is returned to Read or Scanf.  If you want
   * data that is recieved to be returned immediately, set this to 1.
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
   * When set to kFlushOnAccess, data is synchronously written to the serial
   * port after each call to either Printf() or Write().
   *
   * When set to kFlushWhenFull, data will only be written to the serial port
   * when the buffer is full or when Flush() is called.
   *
   * @param mode The write buffer mode.
   */
  void SetWriteBufferMode(WriteBufferMode mode);

  /**
   * Force the output buffer to be written to the port.
   *
   * This is used when SetWriteBufferMode() is set to kFlushWhenFull to force a
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
  hal::Handle<HAL_SerialPortHandle> m_portHandle;
};

}  // namespace frc
