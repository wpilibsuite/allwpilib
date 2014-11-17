#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "CtreCanNode.h"
#include "NetworkCommunication/CANSessionMux.h"
#include <string.h> // memset
#include <unistd.h> // usleep

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
void CtreCanNode::RegisterTx(uint32_t arbId, uint32_t periodMs)
{
	int32_t status = 0;

	txJob_t job = {0};
	job.arbId = arbId;
	job.periodMs = periodMs;
	_txJobs[arbId] = job;
	FRC_NetworkCommunication_CANSessionMux_sendMessage(	job.arbId,
														job.toSend,
														8,
														job.periodMs,
														&status);
}
timespec diff(const timespec & start, const timespec & end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
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
	if(status == 0){
		/* fresh update */
		rxEvent_t & r = _rxRxEvents[arbId]; /* lookup entry or make a default new one with all zeroes */
		clock_gettime(2,&r.time); 			/* fill in time */
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
			struct timespec temp;
			clock_gettime(2,&temp); /* get now */
			/* how long has it been? */
			temp = diff(i->second.time,temp); /* temp = now - last */
			if(temp.tv_sec > 0){
				retval = CTR_RxTimeout;
			}else if(temp.tv_nsec > ((int32_t)timeoutMs*1000*1000)){
				retval = CTR_RxTimeout;
			}else {
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
															8,
															iter->second.periodMs,
															&status);
}

