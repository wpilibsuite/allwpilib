#include <jni.h>
#include <assert.h>
#include "log.h"

#include "edu_wpi_first_wpilibj_hal_SPIJNI.h"


// set the logging level
TLogLevel spiJNILogLevel = logDEBUG;

#define SPIJNI_LOG(level) \
    if (level > spiJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    initializeSPI
 * Signature: (BIBIBILjava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_initializeSPI
  (JNIEnv *, jclass, jbyte, jint, jbyte, jint, jbyte, jint, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    cleanSPI
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_cleanSPI
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPIBitsPerWord
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPIBitsPerWord
  (JNIEnv *, jclass, jobject, jint, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    getSPIBitsPerWord
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_getSPIBitsPerWord
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPIClockRate
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPIClockRate
  (JNIEnv *, jclass, jobject, jdouble, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPIMSBFirst
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPIMSBFirst
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPILSBFirst
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPILSBFirst
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPISampleDataOnFalling
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPISampleDataOnFalling
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPISampleDataOnRising
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPISampleDataOnRising
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPISlaveSelect
 * Signature: (Ljava/nio/ByteBuffer;BILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPISlaveSelect
  (JNIEnv *, jclass, jobject, jbyte, jint, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPILatchMode
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPILatchMode
  (JNIEnv *, jclass, jobject, jint, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    getSPILatchMode
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_getSPILatchMode
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPIFramePolarity
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPIFramePolarity
  (JNIEnv *, jclass, jobject, jbyte, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    getSPIFramePolarity
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_getSPIFramePolarity
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPIClockActiveLow
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPIClockActiveLow
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    setSPIClockActiveHigh
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_setSPIClockActiveHigh
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    applySPIConfig
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_applySPIConfig
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    getSPIOutputFIFOAvailable
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_getSPIOutputFIFOAvailable
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    getSPINumReceived
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)S
 */
JNIEXPORT jshort JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_getSPINumReceived
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    isSPIDone
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_isSPIDone
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    hadSPIReceiveOverflow
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_hadSPIReceiveOverflow
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    writeSPI
 * Signature: (Ljava/nio/ByteBuffer;ILjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_writeSPI
  (JNIEnv *, jclass, jobject, jint, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    readSPI
 * Signature: (Ljava/nio/ByteBuffer;BLjava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_readSPI
  (JNIEnv *, jclass, jobject, jbyte, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    resetSPI
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_resetSPI
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SPIJNI
 * Method:    clearSPIReceivedData
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SPIJNI_clearSPIReceivedData
  (JNIEnv *, jclass, jobject, jobject)
{
	assert(false);
}
