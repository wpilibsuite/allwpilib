/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "edu_wpi_cameraserver_CameraServerJNI.h"

#include "llvm/ArrayRef.h"
#include "llvm/ConvertUTF.h"
#include "llvm/SmallString.h"

#include "cameraserver_cpp.h"

//
// Globals and load/unload
//

// Used for callback.
static JavaVM *jvm = nullptr;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  jvm = vm;

  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;

  jvm = nullptr;
}

}  // extern "C"

//
// Helper class to automatically clean up a local reference
//
template <typename T>
class JavaLocal {
 public:
  JavaLocal(JNIEnv *env, T obj) : m_env(env), m_obj(obj) {}
  ~JavaLocal() {
    if (m_obj) m_env->DeleteLocalRef(m_obj);
  }
  operator T() { return m_obj; }
  T obj() { return m_obj; }

 private:
  JNIEnv *m_env;
  T m_obj;
};

//
// Conversions from Java objects to C++
//

class JavaStringRef {
 public:
  JavaStringRef(JNIEnv *env, jstring str) {
    jsize size = env->GetStringLength(str);
    const jchar *chars = env->GetStringChars(str, nullptr);
    llvm::convertUTF16ToUTF8String(llvm::makeArrayRef(chars, size), m_str);
    env->ReleaseStringChars(str, chars);
  }

  operator llvm::StringRef() const { return m_str; }
  llvm::StringRef str() const { return m_str; }
  const char* c_str() const { return m_str.data(); }

 private:
  llvm::SmallString<128> m_str;
};

//
// Conversions from C++ to Java objects
//

static inline jstring ToJavaString(JNIEnv *env, llvm::StringRef str) {
  llvm::SmallVector<UTF16, 128> chars;
  llvm::convertUTF8ToUTF16String(str, chars);
  return env->NewString(chars.begin(), chars.size());
}

extern "C" {

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getPropertyType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getPropertyType
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getPropertyName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getPropertyName
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getBooleanProperty
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getBooleanProperty
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setBooleanProperty
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setBooleanProperty
  (JNIEnv *, jclass, jint, jboolean);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getDoubleProperty
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getDoubleProperty
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setDoubleProperty
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setDoubleProperty
  (JNIEnv *, jclass, jint, jdouble);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getDoublePropertyMin
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getDoublePropertyMin
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getDoublePropertyMax
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getDoublePropertyMax
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getStringProperty
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getStringProperty
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setStringProperty
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setStringProperty
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getEnumProperty
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getEnumProperty
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setEnumProperty
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setEnumProperty
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getEnumPropertyChoices
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getEnumPropertyChoices
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createUSBSourceDev
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createUSBSourceDev
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createUSBSourcePath
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createUSBSourcePath
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createHTTPSource
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createHTTPSource
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createCvSource
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createCvSource
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceName
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceDescription
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceLastFrameTime
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceLastFrameTime
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceNumChannels
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceNumChannels
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    isSourceConnected
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_isSourceConnected
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceProperty
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateSourceProperties
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateSourceProperties
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    copySource
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_copySource
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    releaseSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_releaseSource
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    notifySourceFrame
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_notifySourceFrame
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    notifySourceError
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_notifySourceError
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSourceConnected
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSourceConnected
  (JNIEnv *, jclass, jint, jboolean);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createSourceProperty
 * Signature: (ILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createSourceProperty
  (JNIEnv *, jclass, jint, jstring, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSourceProperty
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSourceProperty
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSourcePropertyByName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSourcePropertyByName
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createHTTPSink
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createHTTPSink
  (JNIEnv *, jclass, jstring, jstring, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createCvSink
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createCvSink
  (JNIEnv *, jclass, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkName
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkDescription
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSinkSource
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSinkSource
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkSourceProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkSourceProperty
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkSource
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkSource
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    copySink
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_copySink
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    releaseSink
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_releaseSink
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSinkSourceChannel
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSinkSourceChannel
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    sinkWaitForFrame
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_sinkWaitForFrame
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkError
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkError
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSinkEnabled
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSinkEnabled
  (JNIEnv *, jclass, jint, jboolean);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSourceListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSourceListener
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSinkListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSinkListener
  (JNIEnv *, jclass, jint);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateUSBCameras
 * Signature: ()[Ledu/wpi/cameraserver/USBCameraInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateUSBCameras
  (JNIEnv *, jclass);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateSources
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateSources
  (JNIEnv *, jclass);

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateSinks
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateSinks
  (JNIEnv *, jclass);

}  // extern "C"
