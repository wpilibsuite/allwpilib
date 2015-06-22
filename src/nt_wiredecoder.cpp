/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_wiredecoder.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "nt_internal.h"
#include "nt_leb128.h"

using namespace NtImpl;

static double
read_double(char* &buf)
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

WireDecoder::WireDecoder(raw_istream& is, unsigned int proto_rev)
    : m_is(is)
{
    m_allocated = 1024;
    m_buf = (char *)std::malloc(m_allocated);
    m_proto_rev = proto_rev;
    m_error = 0;
}

WireDecoder::~WireDecoder()
{
    std::free(m_buf);
}

bool
WireDecoder::ReadDouble(double *val)
{
    char *buf;
    if (!Read(&buf, 8)) return false;
    *val = read_double(buf);
    return true;
}

void
WireDecoder::Realloc(std::size_t len)
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
WireDecoder::ReadType(NT_Type *type)
{
    unsigned int itype;
    if (!Read8(&itype)) return false;
    switch (itype)
    {
    case 0x00:  *type = NT_BOOLEAN; break;
    case 0x01:  *type = NT_DOUBLE; break;
    case 0x02:  *type = NT_STRING; break;
    case 0x03:  *type = NT_RAW; break;
    case 0x10:  *type = NT_BOOLEAN_ARRAY; break;
    case 0x11:  *type = NT_DOUBLE_ARRAY; break;
    case 0x12:  *type = NT_STRING_ARRAY; break;
    case 0x20:  *type = NT_RPC; break;
    default:
        m_error = "unrecognized value type";
        return false;
    }
    return true;
}

bool
WireDecoder::ReadValue(NT_Type type, NT_Value *value)
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
        if (!ReadDouble(&value->data.v_double)) return false;
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
        char *buf;
        if (!Read(&buf, size)) return false;
        value->data.arr_boolean.arr = (int *)std::malloc(size * sizeof(int));
        for (unsigned int i=0; i<size; ++i)
            value->data.arr_boolean.arr[i] = buf[i] ? 1 : 0;
        break;
    }
    case NT_DOUBLE_ARRAY:
    {
        // size
        unsigned int size;
        if (!Read8(&size)) return false;
        value->data.arr_double.size = size;

        // array values
        char *buf;
        if (!Read(&buf, size*8)) return false;
        value->data.arr_double.arr =
                (double *)std::malloc(size * sizeof(double));
        for (unsigned int i=0; i<size; ++i)
            value->data.arr_double.arr[i] = read_double(buf);
        break;
    }
    case NT_STRING_ARRAY:
    {
        // size
        unsigned int size;
        if (!Read8(&size)) return false;
        value->data.arr_string.size = size;

        // array values
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
WireDecoder::ReadString(NT_String *str)
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
