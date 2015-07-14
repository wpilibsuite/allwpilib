/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_RAW_SOCKET_ISTREAM_H_
#define NT_RAW_SOCKET_ISTREAM_H_

#include "raw_istream.h"

#include "tcpsockets/TCPStream.h"

namespace ntimpl {

class raw_socket_istream : public raw_istream {
 public:
  raw_socket_istream(TCPStream& stream, int timeout = 0)
      : m_stream(stream), m_timeout(timeout) {}
  virtual ~raw_socket_istream();
  virtual bool read(void* data, std::size_t len);
  virtual void close();

 private:
  TCPStream& m_stream;
  int m_timeout;
};

}  // namespace ntimpl

#endif  // NT_RAW_SOCKET_ISTREAM_H_
