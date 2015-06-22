/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_messagewriter.h"

#include <cassert>
#include <cstdlib>

#include "nt_internal.h"
#include "nt_leb128.h"

using namespace NtImpl;

MessageWriter::MessageWriter(unsigned int proto_rev)
{
    m_start = m_cur = (char *)std::malloc(1024);
    m_end = m_start + 1024;
    m_proto_rev = proto_rev;
    m_error = 0;
}

MessageWriter::~MessageWriter()
{
    std::free(m_start);
}

void
MessageWriter::EnsureSlow(std::size_t len)
{
    assert(m_end > m_cur);
    if (static_cast<std::size_t>(m_end - m_cur) >= len)
        return;
    std::size_t pos = m_cur - m_start;
    std::size_t newlen = (m_end - m_start) * 2;
    while (newlen < (pos + len))
        newlen *= 2;
    m_start = (char *)std::realloc(m_start, newlen);
    m_cur = m_start + pos;
    m_end = m_start + newlen;
}

void
MessageWriter::WriteKeepAlive()
{
    Ensure(1);
    Write8(NT_MSG_KEEP_ALIVE);
}

void
MessageWriter::WriteClientHello(const NT_String &self_id)
{
    Ensure(3);
    Write8(NT_MSG_CLIENT_HELLO);
    Write16(m_proto_rev);
    if (m_proto_rev < 0x0300u)
        return;
    WriteString(self_id);
}

void
MessageWriter::WriteProtoUnsup()
{
    Ensure(3);
    Write8(NT_MSG_PROTO_UNSUP);
    Write16(m_proto_rev);
}

void
MessageWriter::WriteServerHelloDone()
{
    Ensure(1);
    Write8(NT_MSG_SERVER_HELLO_DONE);
}

void
MessageWriter::WriteServerHello(unsigned int flags, const NT_String &self_id)
{
    if (m_proto_rev < 0x0300u)
        return; // new message in version 3.0
    Ensure(2);
    Write8(NT_MSG_SERVER_HELLO);
    Write8(flags);
    WriteString(self_id);
}

void
MessageWriter::WriteClientHelloDone()
{
    if (m_proto_rev < 0x0300u)
        return; // new message in version 3.0
    Ensure(1);
    Write8(NT_MSG_CLIENT_HELLO_DONE);
}

void
MessageWriter::WriteEntryAssign(const NT_String &name,
                                unsigned int id,
                                unsigned int seq_num,
                                const NT_Value &value,
                                unsigned int flags)
{
    Ensure(1);
    Write8(NT_MSG_ENTRY_ASSIGN);
    WriteString(name);
    Ensure(6);
    WriteType(value.type);
    Write16(id);
    Write16(seq_num);
    if (m_proto_rev >= 0x0300u)
        Write8(flags);
    WriteValue(value);
}

void
MessageWriter::WriteEntryUpdate(unsigned int id,
                                unsigned int seq_num,
                                const NT_Value &value)
{
    Ensure(6);
    Write8(NT_MSG_ENTRY_UPDATE);
    Write16(id);
    Write16(seq_num);
    if (m_proto_rev >= 0x0300u)
        Write8(value.type);
    WriteValue(value);
}

void
MessageWriter::WriteFlagsUpdate(unsigned int id, unsigned int flags)
{
    if (m_proto_rev < 0x0300u)
        return; // new message in version 3.0
    Ensure(4);
    Write8(NT_MSG_FLAGS_UPDATE);
    Write16(id);
    Write8(flags);
}

void
MessageWriter::WriteEntryDelete(unsigned int id)
{
    if (m_proto_rev < 0x0300u)
        return; // new message in version 3.0
    Ensure(3);
    Write8(NT_MSG_ENTRY_DELETE);
    Write16(id);
}

void
MessageWriter::WriteClearEntries()
{
    if (m_proto_rev < 0x0300u)
        return; // new message in version 3.0

    Ensure(5);
    Write8(NT_MSG_CLEAR_ENTRIES);
    Write32(NT_CLEAR_ALL_MAGIC);
}

void
MessageWriter::WriteExecuteRpc(unsigned int id,
                               unsigned int uid,
                               const NT_Value *params_start,
                               const NT_Value *params_end)
{
    WriteRpc(NT_MSG_EXECUTE_RPC, id, uid, params_start, params_end);
}

void
MessageWriter::WriteRpcResponse(unsigned int id,
                                unsigned int uid,
                                const NT_Value *results_start,
                                const NT_Value *results_end)
{
    WriteRpc(NT_MSG_RPC_RESPONSE, id, uid, results_start, results_end);
}

void
MessageWriter::WriteRpc(unsigned int msg_type,
                        unsigned int id,
                        unsigned int uid,
                        const NT_Value *values_start,
                        const NT_Value *values_end)
{
    if (m_proto_rev < 0x0300u)
        return; // new message in version 3.0

    Ensure(5);
    Write8(msg_type);
    Write16(id);
    Write16(uid);

    unsigned long len = 0;
    for (const NT_Value *value = values_start; value != values_end; ++value)
        len += NT_GetValueSize(value, m_proto_rev);
    WriteULEB128(len);

    for (const NT_Value *value = values_start; value != values_end; ++value)
        WriteValue(*value);
}

void
MessageWriter::WriteULEB128(unsigned long val)
{
    Ensure(size_uleb128(val));
    m_cur += write_uleb128(m_cur, val);
}

void
MessageWriter::WriteType(NT_Type type)
{
    Ensure(1);
    size_t len = NT_WriteType(m_cur, type, m_proto_rev);
    if (len == 0)
        m_error = "unrecognized type";
    m_cur += len;
}

void
MessageWriter::WriteValue(const NT_Value &value)
{
    std::size_t len = NT_GetValueSize(&value, m_proto_rev);
    if (len == 0)
    {
        m_error = "unrecognized type when writing value";
        return;
    }
    Ensure(len);
    m_cur += NT_WriteValue(m_cur, &value, m_proto_rev);
}

void
MessageWriter::WriteString(const NT_String &str)
{
    std::size_t len = NT_GetStringSize(&str, m_proto_rev);
    if (len == 0)
    {
        m_error = "invalid string";
        return;
    }
    Ensure(len);
    m_cur += NT_WriteString(m_cur, &str, m_proto_rev);
}
