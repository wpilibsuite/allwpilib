#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_InterruptJNI.h"
#include "HAL/Interrupts.hpp"
#include "HALUtil.h"

TLogLevel interruptJNILogLevel = logERROR;

#define INTERRUPTJNI_LOG(level) \
	if (level > interruptJNILogLevel) ; \
	else Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    initializeInterrupts
 * Signature: (IZ)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_initializeInterrupts
  (JNIEnv * env, jclass, jint interruptIndex, jboolean watcher)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI initializeInterrupts";
	INTERRUPTJNI_LOG(logDEBUG) << "interruptIndex = " << interruptIndex;
	INTERRUPTJNI_LOG(logDEBUG) << "watcher = " << (bool) watcher;

	int32_t status = 0;
	void* interrupt = initializeInterrupts(interruptIndex, watcher, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << interrupt;
	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

	CheckStatus(env, status);
	return (jlong)interrupt;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    cleanInterrupts
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_cleanInterrupts
  (JNIEnv * env, jclass, jlong interrupt_pointer)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI cleanInterrupts";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	int32_t status = 0;
	cleanInterrupts((void*)interrupt_pointer, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

	CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    waitForInterrupt
 * Signature: (JD)V
 */
JNIEXPORT int JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_waitForInterrupt
  (JNIEnv * env, jclass, jlong interrupt_pointer, jdouble timeout, jboolean ignorePrevious)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI waitForInterrupt";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	int32_t status = 0;
	int result = waitForInterrupt((void*)interrupt_pointer, timeout, ignorePrevious, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

	CheckStatus(env, status);
	return result;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    enableInterrupts
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_enableInterrupts
  (JNIEnv * env, jclass, jlong interrupt_pointer)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI enableInterrupts";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	int32_t status = 0;
	enableInterrupts((void*)interrupt_pointer, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

	CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    disableInterrupts
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_disableInterrupts
  (JNIEnv * env, jclass, jlong interrupt_pointer)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI disableInterrupts";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	int32_t status = 0;
	disableInterrupts((void*)interrupt_pointer, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;

	CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    readRisingTimestamp
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_readRisingTimestamp
  (JNIEnv * env, jclass, jlong interrupt_pointer)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI readRisingTimestamp";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	int32_t status = 0;
	jdouble timeStamp = readRisingTimestamp((void*)interrupt_pointer, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
	return timeStamp;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    readFallingTimestamp
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_readFallingTimestamp
  (JNIEnv * env, jclass, jlong interrupt_pointer)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI readFallingTimestamp";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	int32_t status = 0;
	jdouble timeStamp = readFallingTimestamp((void*)interrupt_pointer, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
	return timeStamp;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    requestInterrupts
 * Signature: (JBIZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_requestInterrupts
  (JNIEnv * env, jclass, jlong interrupt_pointer, jbyte routing_module, jint routing_pin, jboolean routing_analog_trigger)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI requestInterrupts";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;
	INTERRUPTJNI_LOG(logDEBUG) << "routing module = " << (jint) routing_module;
	INTERRUPTJNI_LOG(logDEBUG) << "routing pin = " << routing_pin;
	INTERRUPTJNI_LOG(logDEBUG) << "routing analog trigger = " << (jint) routing_analog_trigger;

	int32_t status = 0;
	requestInterrupts((void*)interrupt_pointer, (uint8_t) routing_module, (uint32_t) routing_pin, routing_analog_trigger, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
}



struct InterruptHandlerParam {
	/*
	 * The object edu/wpi/first/wpilibj/hal/InterruptJNI/InterruptHandlerFunction
	 * that contains the callback method [code]mid[/code].
	 */
	jobject handler_obj;

	//The method id for the callback method
	jmethodID mid;

	//The params passed to the function
	jobject param;

	~InterruptHandlerParam(){
		INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI InterruptHandlerParam Destructor";
		JNIEnv *env;
		jint rs = jvm->AttachCurrentThread((void**)&env, NULL);
		assert (rs == JNI_OK);

		env->DeleteGlobalRef(handler_obj);
		env->DeleteGlobalRef(param);
		rs = jvm->DetachCurrentThread();
		assert (rs == JNI_OK);
		INTERRUPTJNI_LOG(logDEBUG) << "Leaving INTERRUPTJNI InterruptHandlerParam Destructor";
	}
};

void interruptHandler(uint32_t mask, void *data) {
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI interruptHandler";
	InterruptHandlerParam *param = static_cast<InterruptHandlerParam *>(data);

	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam Ptr = " << param;
	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam->obj = " << param->handler_obj;
	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam->param = " << param->param;

	//Because this is a callback in a new thread we must attach it to the JVM
	JNIEnv *env;
	jint rs = jvm->AttachCurrentThread((void**)&env, NULL);
	assert (rs == JNI_OK);
	INTERRUPTJNI_LOG(logDEBUG) << "Attached to thread";

	env->CallVoidMethod(param->handler_obj, param->mid, mask, param->param);
	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}

	rs = jvm->DetachCurrentThread();
	assert (rs == JNI_OK);
	INTERRUPTJNI_LOG(logDEBUG) << "Leaving INTERRUPTJNI interruptHandler";
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    attachInterruptHandler
 * Signature: (JLedu/wpi/first/wpilibj/hal/InterruptJNI/InterruptHandlerFunction;Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_attachInterruptHandler
  (JNIEnv * env, jclass, jlong interrupt_pointer, jobject handler, jobject param)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI attachInterruptHandler";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;

	//Store the interrupt callback paramaters
	InterruptHandlerParam *interruptHandlerParam = new InterruptHandlerParam();
	//Stores the object that contains the callback
	interruptHandlerParam->handler_obj = env->NewGlobalRef(handler);
	//The parameter that will be passed back to the JVM when the method is called
	interruptHandlerParam->param = env->NewGlobalRef(param);

	jclass cls = env->GetObjectClass(handler);
	INTERRUPTJNI_LOG(logDEBUG) << "class = " << cls;
	if (cls == 0) {
		INTERRUPTJNI_LOG(logERROR) << "Error getting java class";
		assert (false);
		return;
	}

	jmethodID mid = env->GetMethodID(cls, "apply", "(ILjava/lang/Object;)V");
	INTERRUPTJNI_LOG(logDEBUG) << "method = " << mid;
	if (mid == 0) {
		INTERRUPTJNI_LOG(logERROR) << "Error getting java method ID";
		assert (false);
		return;
	}
	interruptHandlerParam->mid = mid;

	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam Ptr = " << interruptHandlerParam;
	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam->obj (handler) = " << interruptHandlerParam->handler_obj;
	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam->mid = " << interruptHandlerParam->mid;
	INTERRUPTJNI_LOG(logDEBUG) << "InterruptHandlerParam->param = " << interruptHandlerParam->param;

	int32_t status = 0;
	attachInterruptHandler((void*)interrupt_pointer, interruptHandler, interruptHandlerParam, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    setInterruptUpSourceEdge
 * Signature: (JZZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_setInterruptUpSourceEdge
  (JNIEnv * env, jclass, jlong interrupt_pointer, jboolean risingEdge, jboolean fallingEdge)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI setInterruptUpSourceEdge";
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << (void*)interrupt_pointer;
	INTERRUPTJNI_LOG(logDEBUG) << "Rising Edge = " << (bool) risingEdge;
	INTERRUPTJNI_LOG(logDEBUG) << "Falling Edge = " << (bool) fallingEdge;

	int32_t status = 0;
	setInterruptUpSourceEdge((void*)interrupt_pointer, risingEdge, fallingEdge, &status);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << status;
	CheckStatus(env, status);
}

}  // extern "C"
