/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_WIREENCODER_H_
#define NT_WIREENCODER_H_

#include <cassert>
#include <cstddef>

#include "ntcore.h"

namespace NtImpl {

class WireEncoder
{
public:
    explicit WireEncoder(unsigned int proto_rev);
    ~WireEncoder();

    void SetProtocolRev(unsigned int proto_rev)
    {
        m_proto_rev = proto_rev;
    }

    void Reset()
    {
        m_cur = m_start;
        m_error = 0;
    }

    const char* GetError() const
    {
        return m_error;
    }

    const char* GetData() const
    {
        return m_start;
    }

    std::size_t GetSize() const
    {
        return m_cur - m_start;
    }

    void Reserve(std::size_t len)
    {
        //assert(m_end > m_cur);
        if (static_cast<size_t>(m_end - m_cur) < len)
            ReserveSlow(len);
    }

    void Write8(unsigned int val)
    {
        Reserve(1);
        *m_cur++ = (char)(val & 0xff);
    }

    void Write16(unsigned int val)
    {
        Reserve(2);
        *m_cur++ = (char)((val >> 8) & 0xff);
        *m_cur++ = (char)(val & 0xff);
    }

    void Write32(unsigned long val)
    {
        Reserve(4);
        *m_cur++ = (char)((val >> 24) & 0xff);
        *m_cur++ = (char)((val >> 16) & 0xff);
        *m_cur++ = (char)((val >> 8) & 0xff);
        *m_cur++ = (char)(val & 0xff);
    }

    void WriteDouble(double val);

    void WriteULEB128(unsigned long val);
    void WriteType(NT_Type type);
    void WriteValue(const NT_Value& value);
    void WriteString(const NT_String& str);

    std::size_t GetValueSize(const NT_Value& value);
    std::size_t GetStringSize(const NT_String& str);

protected:
    unsigned int m_proto_rev;
    const char* m_error;

private:
    WireEncoder(const WireEncoder&);
    WireEncoder& operator= (const WireEncoder&);

    void ReserveSlow(std::size_t len);

    char* m_start;
    char* m_cur;
    char* m_end;
};

} // namespace NtImpl

#endif /* NT_WIREENCODER_H_ */
