#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_I2CJNI.h"

#include "HAL/Digital.hpp"

// set the logging level
TLogLevel i2cJNILogLevel = logWARNING;

#define I2CJNI_LOG(level) \
    if (level > i2cJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2cInitialize
 * Signature: (BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CInitialize
  (JNIEnv * env, jclass, jbyte value, jobject status)
{
	I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CInititalize";
	I2CJNI_LOG(logDEBUG) << "Port: " << (jint) value;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	i2CInitialize(value, statusPtr);
	I2CJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CTransaction
 * Signature: (BBLjava/nio/ByteBuffer;BLjava/nio/ByteBuffer;B)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CTransaction
  (JNIEnv * env, jclass, jbyte port, jbyte address, jobject dataToSend, jbyte sendSize, jobject dataReceived, jbyte receiveSize)
{
	I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CTransaction";
	I2CJNI_LOG(logDEBUG) << "Port = " << (jint)port;
	I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
	jbyte * dataToSendPtr = NULL;
	jbyte * dataReceivedPtr = NULL;
	if(dataToSend !=0){
		dataToSendPtr = (jbyte*)env->GetDirectBufferAddress(dataToSend);
	}
	I2CJNI_LOG(logDEBUG) << "DataToSendPtr = " << (jint*)dataToSendPtr;
	I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)sendSize;
	dataReceivedPtr = (jbyte*)env->GetDirectBufferAddress(dataReceived);
	I2CJNI_LOG(logDEBUG) << "DataReceivedPtr = " << (jint*)dataReceivedPtr;
	I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << (jint)receiveSize;
	jbyte returnValue = i2CTransaction(port, address, (uint8_t*)dataToSendPtr, sendSize, (uint8_t*) dataReceivedPtr, receiveSize);
	I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CWrite
 * Signature: (BBLjava/nio/ByteBuffer;B)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CWrite
  (JNIEnv * env, jclass, jbyte port, jbyte address, jobject dataToSend, jbyte sendSize)
{
	
	I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CWrite";
	I2CJNI_LOG(logDEBUG) << "Port = " << (jint)port;
	I2CJNI_LOG(logDEBUG) << "Address = " << (jint)address;
	jbyte * dataToSendPtr = NULL;
	
	if(dataToSend !=0){
		dataToSendPtr = (jbyte*)env->GetDirectBufferAddress(dataToSend);
	}
	I2CJNI_LOG(logDEBUG) << "DataToSendPtr = " << (jint*)dataToSendPtr;
	I2CJNI_LOG(logDEBUG) << "SendSize = " << (jint)dataToSend;
	jbyte returnValue = i2CWrite(port, address, (uint8_t*)dataToSendPtr, sendSize);
	I2CJNI_LOG(logDEBUG) << "ReturnValue = " << (jint)returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CRead
 * Signature: (BBLjava/nio/ByteBuffer;B)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CRead
  (JNIEnv * env, jclass, jbyte port, jbyte address, jobject dataReceived, jbyte receiveSize)
{
	I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2CRead";
	I2CJNI_LOG(logDEBUG) << "Port = " << port;
	I2CJNI_LOG(logDEBUG) << "Address = " << address;
	jbyte * dataReceivedPtr = NULL;
	dataReceivedPtr = (jbyte*)env->GetDirectBufferAddress(dataReceived);
	I2CJNI_LOG(logDEBUG) << "DataReceivedPtr = " << (jint*)dataReceivedPtr;
	I2CJNI_LOG(logDEBUG) << "ReceiveSize = " << receiveSize;
	jbyte returnValue = i2CRead(port, address, (uint8_t*) dataReceivedPtr, receiveSize);
	I2CJNI_LOG(logDEBUG) << "ReturnValue = " << returnValue;
	return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    i2CClose
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_i2CClose
  (JNIEnv *, jclass, jbyte value)
{
	I2CJNI_LOG(logDEBUG) << "Calling I2CJNI i2cClose";
	i2CClose(value);
}
