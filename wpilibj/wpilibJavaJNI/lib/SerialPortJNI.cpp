#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_SerialPortJNI.h"

#include "HAL/SerialPort.hpp"

// set the logging level
TLogLevel serialJNILogLevel = logWARNING;

#define SERIALJNI_LOG(level) \
    if (level > serialJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialInitializePort
 * Signature: (BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialInitializePort
  (JNIEnv * env, jclass, jbyte port, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Calling Serial Initialize";
	SERIALJNI_LOG(logDEBUG) << "Port = " << (jint) port;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialInitializePort(port, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetBaudRate
 * Signature: (BILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetBaudRate
  (JNIEnv * env, jclass, jbyte port, jint rate, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Baud Rate";
	SERIALJNI_LOG(logDEBUG) << "Baud: " << rate;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetBaudRate(port, rate, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetDataBits
 * Signature: (BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetDataBits
  (JNIEnv * env, jclass, jbyte port, jbyte bits, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Data Bits";
	SERIALJNI_LOG(logDEBUG) << "Data Bits: " << bits;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetDataBits(port, bits, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetParity
 * Signature: (BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetParity
  (JNIEnv * env, jclass, jbyte port, jbyte parity, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Parity";
	SERIALJNI_LOG(logDEBUG) << "Parity: " << parity;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetParity(port, parity, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetStopBits
 * Signature: (BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetStopBits
  (JNIEnv * env, jclass, jbyte port, jbyte bits, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Stop Bits";
	SERIALJNI_LOG(logDEBUG) << "Stop Bits: " << bits;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetStopBits(port, bits, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetWriteMode
 * Signature: (BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetWriteMode
  (JNIEnv * env, jclass, jbyte port, jbyte mode, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Mode";
	SERIALJNI_LOG(logDEBUG) << "Write mode: " << mode;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetWriteMode(port, mode, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetFlowControl
 * Signature: (BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetFlowControl
  (JNIEnv * env, jclass, jbyte port, jbyte flow, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Flow Control";
	SERIALJNI_LOG(logDEBUG) << "Flow Control: " << flow;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetFlowControl(port, flow, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetTimeout
 * Signature: (BFLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetTimeout
  (JNIEnv * env, jclass, jbyte port, jfloat timeout, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Timeout";
	SERIALJNI_LOG(logDEBUG) << "Timeout: " << timeout;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetTimeout(port, timeout, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialEnableTermination
 * Signature: (BCLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialEnableTermination
  (JNIEnv * env, jclass, jbyte port, jchar terminator, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Enable Termination";
	SERIALJNI_LOG(logDEBUG) << "Terminator: " << terminator;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialEnableTermination(port, terminator, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialDisableTermination
 * Signature: (BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialDisableTermination
  (JNIEnv * env, jclass, jbyte port, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Disable termination";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialDisableTermination(port, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetReadBufferSize
 * Signature: (BILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetReadBufferSize
  (JNIEnv * env, jclass, jbyte port, jint size, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Read Buffer Size";
	SERIALJNI_LOG(logDEBUG) << "Size: " << size;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetReadBufferSize(port, size, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialSetWriteBufferSize
 * Signature: (BILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialSetWriteBufferSize
  (JNIEnv * env, jclass, jbyte port, jint size, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Setting Serial Write Buffer Size";
	SERIALJNI_LOG(logDEBUG) << "Size: " << size;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialSetWriteBufferSize(port, size, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialGetBytesRecieved
 * Signature: (BLjava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialGetBytesRecieved
  (JNIEnv * env, jclass, jbyte port, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Serial Get Bytes Received";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jint retVal = serialGetBytesReceived(port, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialRead
 * Signature: (BLjava/nio/ByteBuffer;ILjava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialRead
  (JNIEnv * env, jclass, jbyte port, jobject dataReceived, jint size, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Serial Read";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte * dataReceivedPtr = NULL;
	dataReceivedPtr = (jbyte*)env->GetDirectBufferAddress(dataReceived);
	jint retVal = serialRead(port, (char*)dataReceivedPtr, size, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	return retVal;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialWrite
 * Signature: (BLjava/nio/ByteBuffer;ILjava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialWrite
  (JNIEnv * env, jclass, jbyte port, jobject dataToSend, jint size, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Serial Write";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	jbyte * dataToSendPtr = NULL;
	if(dataToSend != 0){
		dataToSendPtr = (jbyte*)env->GetDirectBufferAddress(dataToSend);
	}
	jint retVal = serialWrite(port, (const char*)dataToSendPtr, size, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "ReturnValue = " << retVal;
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	return retVal;
}
	
/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialFlush
 * Signature: (BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialFlush
  (JNIEnv * env, jclass, jbyte port, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Serial Flush";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialFlush(port, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialClear
 * Signature: (BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialClear
  (JNIEnv * env, jclass, jbyte port, jobject status)
{
  	SERIALJNI_LOG(logDEBUG) << "Serial Clear";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialClear(port, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SerialPortJNI
 * Method:    serialClose
 * Signature: (BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SerialPortJNI_serialClose
  (JNIEnv * env, jclass, jbyte port, jobject status)
{
	SERIALJNI_LOG(logDEBUG) << "Serial Close";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	serialClose(port, statusPtr);
	SERIALJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}
