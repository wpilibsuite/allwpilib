/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <llvm/StringRef.h>

#include "ErrorBase.h"

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

  SerialPort(int baudRate, Port port = kOnboard, int dataBits = 8,
             Parity parity = kParity_None, StopBits stopBits = kStopBits_One);
  ~SerialPort();

  SerialPort(const SerialPort&) = delete;
  SerialPort& operator=(const SerialPort&) = delete;

  void SetFlowControl(FlowControl flowControl);
  void EnableTermination(char terminator = '\n');
  void DisableTermination();
  int GetBytesReceived();
  int Read(char* buffer, int count);
  int Write(const char* buffer, int count);
  int Write(llvm::StringRef buffer);
  void SetTimeout(double timeout);
  void SetReadBufferSize(int size);
  void SetWriteBufferSize(int size);
  void SetWriteBufferMode(WriteBufferMode mode);
  void Flush();
  void Reset();

 private:
  int m_resourceManagerHandle = 0;
  int m_portHandle = 0;
  bool m_consoleModeEnabled = false;
  int m_port;
};

}  // namespace frc
