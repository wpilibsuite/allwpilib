#include <jni.h>
#include <assert.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_InterruptJNI.h"
#include "HAL/Interrupts.hpp"

TLogLevel interruptJNILogLevel = logERROR;

#define INTERRUPTJNI_LOG(level) \
	if (level > interruptJNILogLevel) ; \
	else Log().Get(level)

//Used for callback when an interrupt is fired.
static JavaVM *jvm;

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    initializeInterruptJVM
 * Signature: (Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_initializeInterruptJVM
  (JNIEnv * env, jclass, jobject status)
{
	//This method should be called once to setup the JVM
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI initializeInterruptJVM";
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;
	jint rs = env->GetJavaVM(&jvm);
	assert (rs == JNI_OK);
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    initializeInterrupts
 * Signature: (IBLjava/nio/IntBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_initializeInterrupts
  (JNIEnv * env, jclass, jint interruptIndex, jbyte watcher, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI initializeInterrupts";
	INTERRUPTJNI_LOG(logDEBUG) << "interruptIndex = " << interruptIndex;
	INTERRUPTJNI_LOG(logDEBUG) << "watcher = " << (bool) watcher;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	void** interruptPtr = (void**)new unsigned char[4];
	*interruptPtr = (void**) initializeInterrupts(interruptIndex, watcher, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *interruptPtr;
	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

	return env->NewDirectByteBuffer(interruptPtr, 4);
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    cleanInterrupts
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_cleanInterrupts
  (JNIEnv * env, jclass, jobject interrupt_pointer, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI cleanInterrupts";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	cleanInterrupts(*javaId, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    waitForInterrupt
 * Signature: (Ljava/nio/ByteBuffer;DLjava/nio/IntBuffer;)V
 */
JNIEXPORT int JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_waitForInterrupt
  (JNIEnv * env, jclass, jobject interrupt_pointer, jdouble timeout, jboolean ignorePrevious, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI waitForInterrupt";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	int result = waitForInterrupt(*javaId, timeout, ignorePrevious, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;

	return result;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    enableInterrupts
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_enableInterrupts
  (JNIEnv * env, jclass, jobject interrupt_pointer, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI enableInterrupts";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	enableInterrupts(*javaId, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    disableInterrupts
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_disableInterrupts
  (JNIEnv * env, jclass, jobject interrupt_pointer, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI disableInterrupts";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	disableInterrupts(*javaId, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    readRisingTimestamp
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_readRisingTimestamp
  (JNIEnv * env, jclass, jobject interrupt_pointer, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI readRisingTimestamp";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	jdouble timeStamp = readRisingTimestamp(*javaId, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	return timeStamp;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    readFallingTimestamp
 * Signature: (Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_readFallingTimestamp
  (JNIEnv * env, jclass, jobject interrupt_pointer, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI readFallingTimestamp";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	jdouble timeStamp = readFallingTimestamp(*javaId, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
	return timeStamp;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    requestInterrupts
 * Signature: (Ljava/nio/ByteBuffer;BIBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_requestInterrupts
  (JNIEnv * env, jclass, jobject interrupt_pointer, jbyte routing_module, jint routing_pin, jbyte routing_analog_trigger, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI requestInterrupts";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	INTERRUPTJNI_LOG(logDEBUG) << "routing module = " << (jint) routing_module;
	INTERRUPTJNI_LOG(logDEBUG) << "routing pin = " << routing_pin;
	INTERRUPTJNI_LOG(logDEBUG) << "routing analog trigger = " << (jint) routing_analog_trigger;
	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	requestInterrupts(*javaId, (uint8_t) routing_module, (uint32_t) routing_pin, routing_analog_trigger, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
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
 * Signature: (Ljava/nio/ByteBuffer;Ledu/wpi/first/wpilibj/hal/InterruptJNI/InterruptHandlerFunction;Ljava/nio/ByteBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_attachInterruptHandler
  (JNIEnv * env, jclass, jobject interrupt_pointer, jobject handler, jobject param, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI attachInterruptHandler";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

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

	attachInterruptHandler(*javaId, interruptHandler, interruptHandlerParam, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_InterruptJNI
 * Method:    setInterruptUpSourceEdge
 * Signature: (Ljava/nio/ByteBuffer;BBLjava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_InterruptJNI_setInterruptUpSourceEdge
  (JNIEnv * env, jclass, jobject interrupt_pointer, jbyte risingEdge, jbyte fallingEdge, jobject status)
{
	INTERRUPTJNI_LOG(logDEBUG) << "Calling INTERRUPTJNI setInterruptUpSourceEdge";
	void ** javaId = (void**)env->GetDirectBufferAddress(interrupt_pointer);
	INTERRUPTJNI_LOG(logDEBUG) << "Interrupt Ptr = " << *javaId;
	INTERRUPTJNI_LOG(logDEBUG) << "Rising Edge = " << (bool) risingEdge;
	INTERRUPTJNI_LOG(logDEBUG) << "Falling Edge = " << (bool) fallingEdge;

	jint * statusPtr = (jint*)env->GetDirectBufferAddress(status);
	INTERRUPTJNI_LOG(logDEBUG) << "Status Ptr = " << statusPtr;

	setInterruptUpSourceEdge(*javaId, risingEdge, fallingEdge, statusPtr);

	INTERRUPTJNI_LOG(logDEBUG) << "Status = " << *statusPtr;
}
