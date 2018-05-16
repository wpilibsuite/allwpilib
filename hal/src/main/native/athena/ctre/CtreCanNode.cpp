#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "CtreCanNode.h"
#include "FRC_NetworkCommunication/CANSessionMux.h"
#include <string.h> // memset

static const UINT32 kFullMessageIDMask = 0x1fffffff;

CtreCanNode::CtreCanNode(UINT8 deviceNumber)
{
	_deviceNumber = deviceNumber;
}
CtreCanNode::~CtreCanNode()
{
}
void CtreCanNode::RegisterRx(uint32_t arbId)
{
	/* no need to do anything, we just use new API to poll last received message */
}
/**
 * Schedule a CAN Frame for periodic transmit.
 * @param arbId 	CAN Frame Arbitration ID.  Set BIT31 for 11bit ids, otherwise we use 29bit ids.
 * @param periodMs	Period to transmit CAN frame.  Pass 0 for one-shot, which also disables that ArbID's preceding periodic transmit.
 * @param dlc 		Number of bytes to transmit (0 to 8).
 * @param initialFrame	Ptr to the frame data to schedule for transmitting.  Passing null will result
 *						in defaulting to zero data value.
 */
void CtreCanNode::RegisterTx(uint32_t arbId, uint32_t periodMs, uint32_t dlc, const uint8_t * initialFrame)
{
	int32_t status = 0;
	if(dlc > 8)
		dlc = 8;
	txJob_t job = {0};
	job.arbId = arbId;
	job.periodMs = periodMs;
	job.dlc = dlc;
	if(initialFrame){
		/* caller wants to specify original data */
		memcpy(job.toSend, initialFrame, dlc);
	}
	_txJobs[arbId] = job;
	FRC_NetworkCommunication_CANSessionMux_sendMessage(	job.arbId,
														job.toSend,
														job.dlc,
														job.periodMs,
														&status);
}
/**
 * Schedule a CAN Frame for periodic transmit.  Assume eight byte DLC and zero value for initial transmission.
 * @param arbId 	CAN Frame Arbitration ID.  Set BIT31 for 11bit ids, otherwise we use 29bit ids.
 * @param periodMs	Period to transmit CAN frame.  Pass 0 for one-shot, which also disables that ArbID's preceding periodic transmit.
 */
void CtreCanNode::RegisterTx(uint32_t arbId, uint32_t periodMs)
{
	RegisterTx(arbId,periodMs, 8, 0);
}
/**
 * Remove a CAN frame Arbid to stop transmission.
 * @param arbId 	CAN Frame Arbitration ID.  Set BIT31 for 11bit ids, otherwise we use 29bit ids.
 */
void CtreCanNode::UnregisterTx(uint32_t arbId)
{
	/* set period to zero */
	ChangeTxPeriod(arbId, 0);
	/* look and remove */
	txJobs_t::iterator iter = _txJobs.find(arbId);
	if(iter != _txJobs.end()) {
		_txJobs.erase(iter);
	}
}
static int64_t GetTimeMs() {
	std::chrono::time_point < std::chrono::system_clock > now;
	now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast < std::chrono::milliseconds
					> (duration).count();
	return (int64_t) millis;
}
CTR_Code CtreCanNode::GetRx(uint32_t arbId,uint8_t * dataBytes, uint32_t timeoutMs)
{
	CTR_Code retval = CTR_OKAY;
	int32_t status = 0;
	uint8_t len = 0;
	uint32_t timeStamp;
	/* cap timeout at 999ms */
	if(timeoutMs > 999)
		timeoutMs = 999;
	FRC_NetworkCommunication_CANSessionMux_receiveMessage(&arbId,kFullMessageIDMask,dataBytes,&len,&timeStamp,&status);
	std::lock_guard<wpi::mutex> lock(_lck);
	if(status == 0){
		/* fresh update */
		rxEvent_t & r = _rxRxEvents[arbId]; /* lookup entry or make a default new one with all zeroes */
		r.time = GetTimeMs();
		memcpy(r.bytes,  dataBytes,  8);	/* fill in databytes */
	}else{
		/* did not get the message */
		rxRxEvents_t::iterator i = _rxRxEvents.find(arbId);
		if(i == _rxRxEvents.end()){
			/* we've never gotten this mesage */
			retval = CTR_RxTimeout;
			/* fill caller's buffer with zeros */
			memset(dataBytes,0,8);
		}else{
			/* we've gotten this message before but not recently */
			memcpy(dataBytes,i->second.bytes,8);
			/* get the time now */
			int64_t now = GetTimeMs(); /* get now */
			/* how long has it been? */
			int64_t temp = now - i->second.time; /* temp = now - last */
			if (temp > ((int64_t) timeoutMs)) {
					retval = CTR_RxTimeout;
			} else {
					/* our last update was recent enough */
			}
		}
	}

	return retval;
}
void CtreCanNode::FlushTx(uint32_t arbId)
{
	int32_t status = 0;
	txJobs_t::iterator iter = _txJobs.find(arbId);
	if(iter != _txJobs.end())
		FRC_NetworkCommunication_CANSessionMux_sendMessage(	iter->second.arbId,
															iter->second.toSend,
															iter->second.dlc,
															iter->second.periodMs,
															&status);
}
/**
 * Change the transmit period of an already scheduled CAN frame.
 * This keeps the frame payload contents the same without caller having to perform
 * a read-modify-write.
 * @param arbId 	CAN Frame Arbitration ID.  Set BIT31 for 11bit ids, otherwise we use 29bit ids.
 * @param periodMs	Period to transmit CAN frame.  Pass 0 for one-shot, which also disables that ArbID's preceding periodic transmit.
 * @return true if scheduled job was found and updated, false if there was no preceding job for the specified arbID.
 */
bool CtreCanNode::ChangeTxPeriod(uint32_t arbId, uint32_t periodMs)
{
	int32_t status = 0;
	/* lookup the data bytes and period for this message */
	txJobs_t::iterator iter = _txJobs.find(arbId);
	if(iter != _txJobs.end()) {
		/* modify th periodMs */
		iter->second.periodMs = periodMs;
		/* reinsert into scheduler with the same data bytes, only the period changed. */
		FRC_NetworkCommunication_CANSessionMux_sendMessage(	iter->second.arbId,
															iter->second.toSend,
															iter->second.dlc,
															iter->second.periodMs,
															&status);
		return true;
	}
	return false;
}

