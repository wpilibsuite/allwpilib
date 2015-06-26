/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_INTERNAL_H_
#define NT_INTERNAL_H_

namespace ntimpl {

enum MsgType {
  NT_MSG_KEEP_ALIVE = 0x00,
  NT_MSG_CLIENT_HELLO = 0x01,
  NT_MSG_PROTO_UNSUP = 0x02,
  NT_MSG_SERVER_HELLO_DONE = 0x03,
  NT_MSG_SERVER_HELLO = 0x04,
  NT_MSG_CLIENT_HELLO_DONE = 0x05,
  NT_MSG_ENTRY_ASSIGN = 0x10,
  NT_MSG_ENTRY_UPDATE = 0x11,
  NT_MSG_FLAGS_UPDATE = 0x12,
  NT_MSG_ENTRY_DELETE = 0x13,
  NT_MSG_CLEAR_ENTRIES = 0x14,
  NT_MSG_EXECUTE_RPC = 0x20,
  NT_MSG_RPC_RESPONSE = 0x21
};

#define NT_CLEAR_ALL_MAGIC 0xD06CB27Aul

}  // namespace ntimpl

#endif  // NT_INTERNAL_H_
