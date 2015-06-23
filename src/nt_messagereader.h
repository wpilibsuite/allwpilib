/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_MESSAGEREADER_H_
#define NT_MESSAGEREADER_H_

#include "nt_wiredecoder.h"

namespace NtImpl {

class MessageHandler
{
    void anchor();

public:
    // Needed for protocol rev 2.0 ENTRY_UPDATE messages.
    virtual NT_Type GetEntryType(unsigned int id) = 0;

    // All of these functions are expected to take ownership of passed
    // strings/values.
    virtual ~MessageHandler() {}
    virtual void GotKeepAlive() = 0;
    virtual void GotClientHello(unsigned int proto_rev,
                                NT_String& self_id) = 0;
    virtual void GotProtoUnsup(unsigned int proto_rev) = 0;
    virtual void GotServerHelloDone() = 0;
    virtual void GotServerHello(unsigned int flags,
                                NT_String& self_id) = 0;
    virtual void GotClientHelloDone() = 0;
    virtual void GotEntryAssign(NT_String& name,
                                unsigned int id,
                                unsigned int seq_num,
                                NT_Value& value,
                                unsigned int flags) = 0;
    virtual void GotEntryUpdate(unsigned int id,
                                unsigned int seq_num,
                                NT_Value& value) = 0;
    virtual void GotFlagsUpdate(unsigned int id, unsigned int flags) = 0;
    virtual void GotEntryDelete(unsigned int id) = 0;
    virtual void GotClearEntries() = 0;
    virtual void GotExecuteRpc(unsigned int id,
                               unsigned int uid,
                               const char* params,
                               std::size_t params_len) = 0;
    virtual void GotRpcResponse(unsigned int id,
                                unsigned int uid,
                                const char* results,
                                std::size_t results_len) = 0;

private:
    MessageHandler(const MessageHandler&);
    MessageHandler& operator= (const MessageHandler&);
};

class MessageReader : private WireDecoder
{
public:
    explicit MessageReader(MessageHandler &handler,
                           raw_istream& is,
                           unsigned int proto_rev);
    ~MessageReader();

    void SetProtocolRev(unsigned int proto_rev)
    {
        WireDecoder::SetProtocolRev(proto_rev);
    }

    bool Run();

    void Reset()
    {
        WireDecoder::Reset();
    }

    const char* GetError() const
    {
        return WireDecoder::GetError();
    }

private:
    MessageReader(const MessageReader&);
    MessageReader& operator= (const MessageReader&);

    MessageHandler& m_handler;
};

} // namespace NtImpl

#endif /* NT_MESSAGEREADER_H_ */
