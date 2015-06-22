/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_MESSAGEREADER_H_
#define NT_MESSAGEREADER_H_

#include <cstddef>

#include "ntcore.h"
#include "nt_leb128.h"
#include "nt_raw_istream.h"
#include "nt_encoding.h"

namespace NtImpl {

class MessageHandler
{
    void anchor();

public:
    virtual NT_Type GetEntryType(unsigned int id) = 0;
    virtual size_t GetRpcParamTypes(NT_Type *types, unsigned int id) = 0;
    virtual size_t GetRpcResultTypes(NT_Type *types, unsigned int id) = 0;

    // All of these functions are expected to take ownership of passed
    // strings/values.
    virtual ~MessageHandler() {}
    virtual void GotKeepAlive() = 0;
    virtual void GotClientHello(unsigned int proto_rev,
                                NT_String &self_id) = 0;
    virtual void GotProtoUnsup(unsigned int proto_rev) = 0;
    virtual void GotServerHelloDone() = 0;
    virtual void GotServerHello(unsigned int flags,
                                NT_String &self_id) = 0;
    virtual void GotClientHelloDone() = 0;
    virtual void GotEntryAssign(NT_String &name,
                                unsigned int id,
                                unsigned int seq_num,
                                NT_Value &value,
                                unsigned int flags) = 0;
    virtual void GotEntryUpdate(unsigned int id,
                                unsigned int seq_num,
                                NT_Value &value) = 0;
    virtual void GotFlagsUpdate(unsigned int id, unsigned int flags) = 0;
    virtual void GotEntryDelete(unsigned int id) = 0;
    virtual void GotClearEntries() = 0;
    virtual void GotExecuteRpc(unsigned int id,
                               unsigned int uid,
                               NT_Value *params_start,
                               NT_Value *params_end) = 0;
    virtual void GotRpcResponse(unsigned int id,
                                unsigned int uid,
                                NT_Value *results_start,
                                NT_Value *results_end) = 0;

private:
    MessageHandler(const MessageHandler&);
    MessageHandler& operator= (const MessageHandler&);
};

class MessageReader
{
public:
    explicit MessageReader(MessageHandler &handler,
                           raw_istream& is,
                           unsigned int proto_rev);
    ~MessageReader();

    void SetProtocolRev(unsigned int proto_rev)
    {
        m_proto_rev = proto_rev;
    }

    bool Run();

    void Reset()
    {
        m_error = 0;
    }

    const char *GetError() const
    {
        return m_error;
    }

protected:
    bool Read(std::size_t len)
    {
        if (len > m_allocated)
            Realloc(len);
        return m_is.read(m_buf, len);
    }

    bool Read8(unsigned int *val)
    {
        if (!Read(1)) return false;
        char *buf = m_buf;
        *val = NtImpl::Read8(buf);
        return true;
    }

    bool Read16(unsigned int *val)
    {
        if (!Read(2)) return false;
        char *buf = m_buf;
        *val = NtImpl::Read16(buf);
        return true;
    }

    bool Read32(unsigned long *val)
    {
        if (!Read(4)) return false;
        char *buf = m_buf;
        *val = NtImpl::Read32(buf);
        return true;
    }

    bool ReadULEB128(unsigned long *val)
    {
        return read_uleb128(m_is, val);
    }

    bool ReadType(NT_Type *type);
    bool ReadValue(NT_Type type, NT_Value *value);
    bool ReadString(NT_String *str);

private:
    MessageReader(const MessageReader&);
    MessageReader& operator= (const MessageReader&);

    void Realloc(std::size_t len);

    MessageHandler &m_handler;
    raw_istream &m_is;

    char *m_buf;
    std::size_t m_allocated;

    unsigned int m_proto_rev;
    const char *m_error;
    NT_Type m_rpc_types[256];
};

} // namespace NtImpl

#endif /* NT_MESSAGEREADER_H_ */
