/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_messagereader.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "nt_internal.h"
#include "nt_leb128.h"

using namespace NtImpl;

void
MessageHandler::anchor()
{
}

MessageReader::MessageReader(MessageHandler &handler,
                             raw_istream& is,
                             unsigned int proto_rev)
    : m_handler(handler)
    , m_is(is)
{
    m_allocated = 1024;
    m_buf = (char *)std::malloc(m_allocated);
    m_proto_rev = proto_rev;
    m_error = 0;
}

MessageReader::~MessageReader()
{
    std::free(m_buf);
}

void
MessageReader::Realloc(std::size_t len)
{
    if (m_allocated >= len)
        return;
    std::size_t newlen = m_allocated * 2;
    while (newlen < len)
        newlen *= 2;
    m_buf = (char *)std::realloc(m_buf, newlen);
    m_allocated = newlen;
}

bool
MessageReader::Run()
{
    unsigned int msg_type;
    if (!Read8(&msg_type)) return false;
    switch (msg_type)
    {
    case NT_MSG_KEEP_ALIVE:
        m_handler.GotKeepAlive();
        break;
    case NT_MSG_CLIENT_HELLO:
    {
        unsigned int proto_rev;
        if (!Read16(&proto_rev)) return false;
        NT_String self_id;
        NT_InitString(&self_id);
        // This intentionally uses the provided proto_rev instead of
        // m_proto_rev.
        if (proto_rev >= 0x0300u)
        {
            if (!ReadString(&self_id)) return false;
        }
        m_handler.GotClientHello(proto_rev, self_id);
        break;
    }
    case NT_MSG_PROTO_UNSUP:
    {
        unsigned int proto_rev;
        if (!Read16(&proto_rev)) return false;
        m_handler.GotProtoUnsup(proto_rev);
        break;
    }
    case NT_MSG_SERVER_HELLO_DONE:
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received SERVER_HELLO_DONE in protocol < 3.0";
            return false;
        }
        m_handler.GotServerHelloDone();
        break;
    case NT_MSG_CLIENT_HELLO_DONE:
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received CLIENT_HELLO_DONE in protocol < 3.0";
            return false;
        }
        m_handler.GotClientHelloDone();
        break;
    case NT_MSG_ENTRY_ASSIGN:
    {
        NT_String name;
        if (!ReadString(&name)) return false;
        NT_Type type;
        unsigned int id, seq_num;
        unsigned int flags = 0;
        NT_Value value;
        if (!ReadType(&type) ||
            !Read16(&id) ||
            !Read16(&seq_num) ||
            (m_proto_rev >= 0x0300u && !Read8(&flags)) ||
            !ReadValue(type, &value))
        {
            NT_DisposeString(&name);
            return false;
        }
        m_handler.GotEntryAssign(name, id, seq_num, value, flags);
        break;
    }
    case NT_MSG_ENTRY_UPDATE:
    {
        unsigned int id, seq_num;
        NT_Value value;
        if (!Read16(&id)) return false;
        if (!Read16(&seq_num)) return false;
        NT_Type type;
        if (m_proto_rev >= 0x0300u)
        {
            unsigned int itype;
            if (!Read8(&itype)) return false;
            type = static_cast<NT_Type>(itype);
        }
        else
            type = m_handler.GetEntryType(id);
        if (!ReadValue(type, &value)) return false;
            m_handler.GotEntryUpdate(id, seq_num, value);
        break;
    }
    case NT_MSG_FLAGS_UPDATE:
    {
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received FLAGS_UPDATE in protocol < 3.0";
            return false;
        }
        unsigned int id, flags;
        if (!Read16(&id)) return false;
        if (!Read8(&flags)) return false;
        m_handler.GotFlagsUpdate(id, flags);
        break;
    }
    case NT_MSG_ENTRY_DELETE:
    {
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received ENTRY_DELETE in protocol < 3.0";
            return false;
        }
        unsigned int id;
        if (!Read16(&id)) return false;
        m_handler.GotEntryDelete(id);
        break;
    }
    case NT_MSG_CLEAR_ENTRIES:
    {
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received CLEAR_ENTRIES in protocol < 3.0";
            return false;
        }
        unsigned long magic;
        if (!Read32(&magic)) return false;
        if (magic != NT_CLEAR_ALL_MAGIC)
        {
            m_error = "received incorrect CLEAR_ENTRIES magic value, ignoring";
            return true;
        }
        m_handler.GotClearEntries();
        break;
    }
    case NT_MSG_EXECUTE_RPC:
    {
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received EXECUTE_RPC in protocol < 3.0";
            return false;
        }
        unsigned int id, uid;
        if (!Read16(&id)) return false;
        if (!Read16(&uid)) return false;
        unsigned long size;
        if (!ReadULEB128(&size)) return false;
        if (!Read(size)) return false;
    }
    case NT_MSG_RPC_RESPONSE:
    default:
        m_error = "unrecognized message type";
        return false;
    }
    return true;
}

bool
MessageReader::ReadType(NT_Type *type)
{
    if (!Read(1)) return false;
    size_t len = NT_ReadType(m_buf, type, m_proto_rev);
    if (len == 0)
    {
        m_error = "unrecognized value type";
        return false;
    }
    return true;
}

bool
MessageReader::ReadValue(NT_Type type, NT_Value *value)
{
    value->type = type;
    value->last_change = 0;
    switch (type)
    {
    case NT_BOOLEAN:
    {
        unsigned int v;
        if (!Read8(&v)) return false;
        value->data.v_boolean = v ? 1 : 0;
        break;
    }
    case NT_DOUBLE:
    {
        if (!Read(8)) return false;
        char *buf = m_buf;
        value->data.v_double = ReadDouble(buf);
        break;
    }
    case NT_STRING:
        if (!ReadString(&value->data.v_string)) return false;
        break;
    case NT_RAW:
    case NT_RPC:
        if (m_proto_rev < 0x0300u)
        {
            m_error = "received raw or RPC value in protocol < 3.0";
            return false;
        }
        if (!ReadString(&value->data.v_raw)) return false;
        break;
    case NT_BOOLEAN_ARRAY:
    {
        // size
        unsigned int size;
        if (!Read8(&size)) return false;
        value->data.arr_boolean.size = size;

        // array values
        if (!Read(size)) return false;
        value->data.arr_boolean.arr = (int *)std::malloc(size * sizeof(int));
        for (unsigned int i=0; i<size; ++i)
            value->data.arr_boolean.arr[i] = m_buf[i] ? 1 : 0;
        break;
    }
    case NT_DOUBLE_ARRAY:
    {
        // size
        unsigned int size;
        if (!Read8(&size)) return false;
        value->data.arr_double.size = size;

        // array values
        if (!Read(size*8)) return false;
        value->data.arr_double.arr =
                (double *)std::malloc(size * sizeof(double));
        char *buf = m_buf;
        for (unsigned int i=0; i<size; ++i)
            value->data.arr_double.arr[i] = ReadDouble(buf);
        break;
    }
    case NT_STRING_ARRAY:
    {
        // size
        unsigned int size;
        if (!Read8(&size)) return false;
        value->data.arr_string.size = size;

        // array values
        if (!Read(size*8)) return false;
        value->data.arr_string.arr =
                (NT_String *)std::malloc(size * sizeof(NT_String));
        for (unsigned int i=0; i<size; ++i)
        {
            if (!ReadString(&value->data.arr_string.arr[i]))
            {
                // cleanup to avoid memory leaks
                for (unsigned int j=0; j<i; ++j)
                {
                    std::free(value->data.arr_string.arr[j].str);
                }
                std::free(value->data.arr_string.arr);
                return false;
            }
        }
        break;
    }
    default:
        m_error = "invalid type when trying to read value";
        return false;
    }
    return true;
}

bool
MessageReader::ReadString(NT_String *str)
{
    if (m_proto_rev < 0x0300u)
    {
        unsigned int v;
        if (!Read16(&v)) return false;
        str->len = v;
    }
    else
    {
        unsigned long v;
        if (!ReadULEB128(&v)) return false;
        str->len = v;
    }
    str->str = (char *)std::malloc(str->len + 1);
    if (!m_is.read(str->str, str->len))
    {
        std::free(str->str);
        str->str = 0;
        return false;
    }
    str->str[str->len] = '\0';
    return true;
}
