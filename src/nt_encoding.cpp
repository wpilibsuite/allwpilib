/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_encoding.h"
#include "ntcore.h"

#include <cstdint>
#include <cstring>

#include "nt_leb128.h"

using namespace NtImpl;

void
NtImpl::WriteDouble(char* &buf, double val)
{
    std::uint64_t v = *reinterpret_cast<uint64_t*>(&val);
    *buf++ = (char)((v >> 56) & 0xff);
    *buf++ = (char)((v >> 48) & 0xff);
    *buf++ = (char)((v >> 40) & 0xff);
    *buf++ = (char)((v >> 32) & 0xff);
    *buf++ = (char)((v >> 24) & 0xff);
    *buf++ = (char)((v >> 16) & 0xff);
    *buf++ = (char)((v >> 8) & 0xff);
    *buf++ = (char)(v & 0xff);
}

double
NtImpl::ReadDouble(char* &buf)
{
    std::uint64_t val = (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf; val <<= 8; val |= (*((unsigned char *)buf)) & 0xff;
    ++buf;
    return *reinterpret_cast<double*>(&val);
}

size_t
NT_WriteType(char *buf, NT_Type type, unsigned int proto_rev)
{
    switch (type)
    {
    case NT_BOOLEAN:        *buf = 0x00; break;
    case NT_DOUBLE:         *buf = 0x01; break;
    case NT_STRING:         *buf = 0x02; break;
    case NT_RAW:
        if (proto_rev < 0x0300u)
            return 0;
        *buf = 0x03;
        break;
    case NT_BOOLEAN_ARRAY:  *buf = 0x10; break;
    case NT_DOUBLE_ARRAY:   *buf = 0x11; break;
    case NT_STRING_ARRAY:   *buf = 0x12; break;
    case NT_RPC:
        if (proto_rev < 0x0300u)
            return 0;
        *buf = 0x20;
        break;
    default:
        return 0;
    }
    return 1;
}

size_t
NT_ReadType(const char *buf, NT_Type *type, unsigned int proto_rev)
{
    switch (buf[0])
    {
    case 0x00:  *type = NT_BOOLEAN; break;
    case 0x01:  *type = NT_DOUBLE; break;
    case 0x02:  *type = NT_STRING; break;
    case 0x03:  *type = NT_RAW; break;
    case 0x10:  *type = NT_BOOLEAN_ARRAY; break;
    case 0x11:  *type = NT_DOUBLE_ARRAY; break;
    case 0x12:  *type = NT_STRING_ARRAY; break;
    case 0x20:  *type = NT_RPC; break;
    default:    return 0;
    }
    return 1;
}

size_t
NT_GetStringSize(const NT_String *str, unsigned int proto_rev)
{
    if (proto_rev < 0x0300u)
        return 2 + str->len;
    return size_uleb128(str->len) + str->len;
}

size_t
NT_WriteString(char *buf, const NT_String *str, unsigned int proto_rev)
{
    char *start = buf;
    if (proto_rev < 0x0300u)
        Write16(buf, str->len);
    else
        buf += write_uleb128(buf, str->len);
    std::memcpy(buf, str->str, str->len);
    return (buf - start) + str->len;
}

size_t
NT_GetValueSize(const NT_Value *value, unsigned int proto_rev)
{
    switch (value->type)
    {
    case NT_BOOLEAN:
        return 1;
    case NT_DOUBLE:
        return 8;
    case NT_STRING:
        return NT_GetStringSize(&value->data.v_string, proto_rev);
    case NT_RAW:
    case NT_RPC:
        if (proto_rev < 0x0300u)
            return 0;
        return NT_GetStringSize(&value->data.v_raw, proto_rev);
    case NT_BOOLEAN_ARRAY:
        return 1 + value->data.arr_boolean.size;
    case NT_DOUBLE_ARRAY:
        return 1 + value->data.arr_double.size * 8;
    case NT_STRING_ARRAY:
    {
        size_t len = 1;
        for (size_t i=0; i<value->data.arr_string.size; ++i)
            len += NT_GetStringSize(&value->data.arr_string.arr[i], proto_rev);
        return len;
    }
    default:
        return 0;
    }
    return 1;
}

size_t
NT_WriteValue(char *buf, const NT_Value *value, unsigned int proto_rev)
{
    char *start = buf;
    switch (value->type)
    {
    case NT_BOOLEAN:
        Write8(buf, value->data.v_boolean ? 1 : 0);
        break;
    case NT_DOUBLE:
        WriteDouble(buf, value->data.v_double);
        break;
    case NT_STRING:
        buf += NT_WriteString(buf, &value->data.v_string, proto_rev);
        break;
    case NT_RAW:
    case NT_RPC:
        if (proto_rev < 0x0300u)
            return 0;
        buf += NT_WriteString(buf, &value->data.v_raw, proto_rev);
        break;
    case NT_BOOLEAN_ARRAY:
        Write8(buf, value->data.arr_boolean.size);
        for (size_t i=0; i<value->data.arr_boolean.size; ++i)
            Write8(buf, value->data.arr_boolean.arr[i] ? 1 : 0);
        break;
    case NT_DOUBLE_ARRAY:
        Write8(buf, value->data.arr_double.size);
        for (size_t i=0; i<value->data.arr_double.size; ++i)
            WriteDouble(buf, value->data.arr_double.arr[i] ? 1 : 0);
        break;
    case NT_STRING_ARRAY:
        Write8(buf, value->data.arr_string.size);
        for (size_t i=0; i<value->data.arr_string.size; ++i)
            buf += NT_WriteString(buf, &value->data.arr_string.arr[i],
                                  proto_rev);
        break;
    default:
        return 0;
    }
    return buf - start;
}
