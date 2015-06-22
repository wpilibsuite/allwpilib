/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_MESSAGEWRITER_H_
#define NT_MESSAGEWRITER_H_

#include <cassert>
#include <cstddef>

#include "ntcore.h"
#include "nt_encoding.h"

namespace NtImpl {

class MessageWriter
{
public:
    explicit MessageWriter(unsigned int proto_rev);
    ~MessageWriter();

    void SetProtocolRev(unsigned int proto_rev)
    {
        m_proto_rev = proto_rev;
    }

    void Reset()
    {
        m_cur = m_start;
        m_error = 0;
    }

    const char *GetError() const
    {
        return m_error;
    }

    const char *GetData() const
    {
        return m_start;
    }

    std::size_t GetSize() const
    {
        return m_cur - m_start;
    }

    void WriteKeepAlive();
    void WriteClientHello(const NT_String &self_id);
    void WriteProtoUnsup();
    void WriteServerHelloDone();
    void WriteServerHello(unsigned int flags, const NT_String &self_id);
    void WriteClientHelloDone();
    void WriteEntryAssign(const NT_String &name,
                          unsigned int id,
                          unsigned int seq_num,
                          const NT_Value &value,
                          unsigned int flags);
    void WriteEntryUpdate(unsigned int id,
                          unsigned int seq_num,
                          const NT_Value &value);
    void WriteFlagsUpdate(unsigned int id, unsigned int flags);
    void WriteEntryDelete(unsigned int id);
    void WriteClearEntries();
    void WriteExecuteRpc(unsigned int id,
                         unsigned int uid,
                         const NT_Value *params_start,
                         const NT_Value *params_end);
    void WriteRpcResponse(unsigned int id,
                          unsigned int uid,
                          const NT_Value *results_start,
                          const NT_Value *results_end);

protected:
    void Ensure(std::size_t len)
    {
        assert(m_end > m_cur);
        if (static_cast<size_t>(m_end - m_cur) < len)
            EnsureSlow(len);
    }

    void Write8(unsigned int val)
    {
        NtImpl::Write8(m_cur, val);
    }

    void Write16(unsigned int val)
    {
        NtImpl::Write16(m_cur, val);
    }

    void Write32(unsigned long val)
    {
        NtImpl::Write32(m_cur, val);
    }

    void WriteULEB128(unsigned long val);
    void WriteType(NT_Type type);
    void WriteValue(const NT_Value &value);
    void WriteString(const NT_String &str);

private:
    MessageWriter(const MessageWriter&);
    MessageWriter& operator= (const MessageWriter&);

    void EnsureSlow(std::size_t len);
    void WriteRpc(unsigned int msg_type,
                  unsigned int id,
                  unsigned int uid,
                  const NT_Value *values_start,
                  const NT_Value *values_end);

    char *m_start;
    char *m_cur;
    char *m_end;
    unsigned int m_proto_rev;
    const char *m_error;
};

} // namespace NtImpl

#endif /* NT_MESSAGEWRITER_H_ */
