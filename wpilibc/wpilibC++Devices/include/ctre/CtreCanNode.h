#ifndef CtreCanNode_H_
#define CtreCanNode_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include <NetworkCommunication/CANSessionMux.h>	//CAN Comm
#include <pthread.h>
#include <map>
#include <string.h> // memcpy
#include <sys/time.h>
class CtreCanNode
{
public:
	CtreCanNode(UINT8 deviceNumber);
    ~CtreCanNode();

	UINT8 GetDeviceNumber()
	{
		return _deviceNumber;
	}
protected:


	template <typename T> class txTask{
		public:
			uint32_t arbId;
			T * toSend;
			T * operator -> ()
			{
				return toSend;
			}
			T & operator*()
			{
				return *toSend;
			}
			bool IsEmpty()
			{
				if(toSend == 0)
					return true;
				return false;
			}
	};
	template <typename T> class recMsg{
		public:
			uint32_t arbId;
			uint8_t bytes[8];
			CTR_Code err;
			T * operator -> ()
			{
				return (T *)bytes;
			}
			T & operator*()
			{
				return *(T *)bytes;
			}
	};
	UINT8 _deviceNumber;
	void RegisterRx(uint32_t arbId);
	void RegisterTx(uint32_t arbId, uint32_t periodMs);

	CTR_Code GetRx(uint32_t arbId,uint8_t * dataBytes,uint32_t timeoutMs);
	void FlushTx(uint32_t arbId);

	template<typename T> txTask<T> GetTx(uint32_t arbId)
	{
		txTask<T> retval = {0};
		txJobs_t::iterator i = _txJobs.find(arbId);
		if(i != _txJobs.end()){
			retval.arbId = i->second.arbId;
			retval.toSend = (T*)i->second.toSend;
		}
		return retval;
	}
	template<class T> void FlushTx(T & par)
	{
		FlushTx(par.arbId);
	}

	template<class T> recMsg<T> GetRx(uint32_t arbId, uint32_t timeoutMs)
	{
		recMsg<T> retval;
		retval.err = GetRx(arbId,retval.bytes, timeoutMs);
		return retval;
	}

private:

	class txJob_t {
		public:
			uint32_t arbId;
			uint8_t toSend[8];
			uint32_t periodMs;
	};

	class rxEvent_t{
		public:
			uint8_t bytes[8];
			struct timespec time;
			rxEvent_t()
			{
				bytes[0] = 0;
				bytes[1] = 0;
				bytes[2] = 0;
				bytes[3] = 0;
				bytes[4] = 0;
				bytes[5] = 0;
				bytes[6] = 0;
				bytes[7] = 0;
			}
	};

	typedef std::map<uint32_t,txJob_t> txJobs_t;
	txJobs_t _txJobs;

	typedef std::map<uint32_t,rxEvent_t> rxRxEvents_t;
	rxRxEvents_t _rxRxEvents;
};
#endif
