#include <jni.h>
#include <assert.h>
#include "log.h"

#include "edu_wpi_first_wpilibj_hal_I2CJNI.h"


// set the logging level
TLogLevel i2cJNILogLevel = logDEBUG;

#define I2CJNI_LOG(level) \
    if (level > i2cJNILogLevel) ; \
    else Log().Get(level)

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    doI2CTransaction
 * Signature: (BBLjava/nio/ByteBuffer;BLjava/nio/ByteBuffer;BLjava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_doI2CTransaction
  (JNIEnv *, jclass, jbyte, jbyte, jobject, jbyte, jobject, jbyte, jobject)
{
	assert(false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_I2CJNI
 * Method:    doI2CTransactionWithModule
 * Signature: (BBBLjava/nio/ByteBuffer;BLjava/nio/ByteBuffer;BLjava/nio/IntBuffer;)B
 */
JNIEXPORT jbyte JNICALL Java_edu_wpi_first_wpilibj_hal_I2CJNI_doI2CTransactionWithModule
  (JNIEnv *, jclass, jbyte, jbyte, jbyte, jobject, jbyte, jobject, jbyte, jobject)
{
	assert(false);
}
