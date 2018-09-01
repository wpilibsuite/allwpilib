/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/CAN.h"
#include "MauInternal.h"
#include <VMXCAN.h>

namespace hal {
    namespace init {
        void InitializeCAN() {}
    }
}

extern "C" {
    void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data, uint8_t dataSize, int32_t periodMs, int32_t* status) {
//        VMXCANMessage message = VMXCANMessage();
//        VMXErrorCode* error;
//        message.messageID = messageID;
//        message.setData(data, dataSize);
//        mau::vmxCAN->SendMessage(message, periodMs, error);
    }

    void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask, uint8_t* data, uint8_t* dataSize,
                                uint32_t* timeStamp, int32_t* status) {
        // TODO: ReceiveMessage functionality to VMX-pi HAL [Issue: #93]
    }

    void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID, uint32_t messageIDMask, uint32_t maxMessages,
                                   int32_t* status) {
//        VMXErrorCode* error;
//        VMXCANReceiveStreamHandle handle = (VMXCANReceiveStreamHandle) sessionHandle;
//        mau::vmxCAN->OpenReceiveStream(handle, messageID, messageIDMask, maxMessages, error);
    }

    void HAL_CAN_CloseStreamSession(uint32_t sessionHandle) {
//        VMXErrorCode* error;
//        mau::vmxCAN->CloseReceiveStream(sessionHandle, error);
    }

    void HAL_CAN_ReadStreamSession(uint32_t sessionHandle, struct HAL_CANStreamMessage* messages, uint32_t messagesToRead,
                                   uint32_t* messagesRead, int32_t* status) {
//        VMXErrorCode* error;
//        VMXCANTimestampedMessage vmxMessage = VMXCANTimestampedMessage();
//        vmxMessage.messageID = messages->messageID;
//        vmxMessage.timeStampMS = messages->timeStamp;
//        vmxMessage.setData(messages->data, messages->dataSize);
//        mau::vmxCAN->ReadReceiveStream(sessionHandle, &vmxMessage, messagesToRead, *messagesRead, error);
    }

    void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount, uint32_t* txFullCount,
                              uint32_t* receiveErrorCount, uint32_t* transmitErrorCount, int32_t* status) {
//        VMXErrorCode* error;
//        VMXCANBusStatus busStatus = VMXCANBusStatus();
//        busStatus.percentBusUtilization = *percentBusUtilization;
//        busStatus.busOffCount = *busOffCount;
//        busStatus.txFullCount = *txFullCount;
//        busStatus.receiveErrorCount = *receiveErrorCount;
//        busStatus.transmitErrorCount = *transmitErrorCount;
//        mau::vmxCAN->GetCANBUSStatus(busStatus, error);
    }
}
