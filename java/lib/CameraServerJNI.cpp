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
static jclass stringCls = nullptr;
static jclass usbCameraInfoCls = nullptr;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  jvm = vm;

  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  // Cache references to classes
  jclass local;

  local = env->FindClass("java/lang/String");
  if (!local) return JNI_ERR;
  stringCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!stringCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("edu/wpi/cameraserver/USBCameraInfo");
  if (!local) return JNI_ERR;
  usbCameraInfoCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!usbCameraInfoCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;
  // Delete global references
  if (stringCls) env->DeleteGlobalRef(stringCls);
  if (usbCameraInfoCls) env->DeleteGlobalRef(usbCameraInfoCls);
  jvm = nullptr;
}

}  // extern "C"

static void ReportError(JNIEnv *env, CS_Status status, bool do_throw = true) {
  // TODO
}

static inline bool CheckStatus(JNIEnv *env, CS_Status status,
                               bool do_throw = true) {
  if (status != 0) ReportError(env, status, do_throw);
  return status == 0;
}

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

namespace {

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

}  // anonymous namespace

//
// Conversions from C++ to Java objects
//

static inline jstring ToJavaString(JNIEnv *env, llvm::StringRef str) {
  llvm::SmallVector<UTF16, 128> chars;
  llvm::convertUTF8ToUTF16String(str, chars);
  return env->NewString(chars.begin(), chars.size());
}

static jintArray ToJavaIntArray(JNIEnv *env, llvm::ArrayRef<int> arr)
{
  jintArray jarr = env->NewIntArray(arr.size());
  if (!jarr) return nullptr;
  env->SetIntArrayRegion(jarr, 0, arr.size(), arr.data());
  return jarr;
}

static jobjectArray ToJavaStringArray(JNIEnv *env,
                                      llvm::ArrayRef<std::string> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), stringCls, nullptr);
  if (!jarr) return nullptr;
  for (std::size_t i = 0; i < arr.size(); ++i) {
    JavaLocal<jstring> elem{env, ToJavaString(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobject ToJavaObject(JNIEnv *env, const cs::USBCameraInfo &info) {
  static jmethodID constructor = env->GetMethodID(
      usbCameraInfoCls, "<init>", "(ILjava/lang/String;Ljava/lang/String;I)V");
  JavaLocal<jstring> path(env, ToJavaString(env, info.path));
  JavaLocal<jstring> name(env, ToJavaString(env, info.name));
  return env->NewObject(usbCameraInfoCls, constructor,
                        static_cast<jint>(info.dev), path.obj(), name.obj(),
                        static_cast<jint>(info.channels));
}

extern "C" {

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getPropertyType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getPropertyType
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto val = cs::GetPropertyType(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getPropertyName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getPropertyName
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetPropertyName(property, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getBooleanProperty
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getBooleanProperty
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto val = cs::GetBooleanProperty(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setBooleanProperty
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setBooleanProperty
  (JNIEnv *env, jclass, jint property, jboolean value)
{
  CS_Status status;
  cs::SetBooleanProperty(property, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getDoubleProperty
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getDoubleProperty
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto val = cs::GetDoubleProperty(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setDoubleProperty
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setDoubleProperty
  (JNIEnv *env, jclass, jint property, jdouble value)
{
  CS_Status status;
  cs::SetDoubleProperty(property, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getDoublePropertyMin
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getDoublePropertyMin
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto val = cs::GetDoublePropertyMin(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getDoublePropertyMax
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getDoublePropertyMax
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto val = cs::GetDoublePropertyMax(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getStringProperty
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getStringProperty
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetStringProperty(property, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setStringProperty
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setStringProperty
  (JNIEnv *env, jclass, jint property, jstring value)
{
  CS_Status status;
  cs::SetStringProperty(property, JavaStringRef{env, value}, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getEnumProperty
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getEnumProperty
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto val = cs::GetEnumProperty(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setEnumProperty
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setEnumProperty
  (JNIEnv *env, jclass, jint property, jint value)
{
  CS_Status status;
  cs::SetEnumProperty(property, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getEnumPropertyChoices
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getEnumPropertyChoices
  (JNIEnv *env, jclass, jint property)
{
  CS_Status status;
  auto arr = cs::GetEnumPropertyChoices(property, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaStringArray(env, arr);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createUSBSourceDev
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createUSBSourceDev
  (JNIEnv *env, jclass, jstring name, jint dev)
{
  CS_Status status;
  auto val = cs::CreateUSBSourceDev(JavaStringRef{env, name}, dev, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createUSBSourcePath
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createUSBSourcePath
  (JNIEnv *env, jclass, jstring name, jstring path)
{
  CS_Status status;
  auto val = cs::CreateUSBSourcePath(JavaStringRef{env, name},
                                     JavaStringRef{env, path}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createHTTPSource
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createHTTPSource
  (JNIEnv *env, jclass, jstring name, jstring url)
{
  CS_Status status;
  auto val = cs::CreateHTTPSource(JavaStringRef{env, name},
                                  JavaStringRef{env, url}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createCvSource
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createCvSource
  (JNIEnv *env, jclass, jstring name, jint numChannels)
{
  CS_Status status;
  auto val = cs::CreateCvSource(JavaStringRef{env, name}, numChannels, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceName
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetSourceName(source, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceDescription
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetSourceDescription(source, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceLastFrameTime
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceLastFrameTime
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  auto val = cs::GetSourceLastFrameTime(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceNumChannels
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceNumChannels
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  auto val = cs::GetSourceNumChannels(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    isSourceConnected
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_isSourceConnected
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  auto val = cs::IsSourceConnected(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSourceProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSourceProperty
  (JNIEnv *env, jclass, jint source, jstring name)
{
  CS_Status status;
  auto val = cs::GetSourceProperty(source, JavaStringRef{env, name}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateSourceProperties
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateSourceProperties
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  llvm::SmallVector<CS_Property, 32> arr;
  cs::EnumerateSourceProperties(source, arr, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaIntArray(env, arr);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    copySource
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_copySource
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  auto val = cs::CopySource(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    releaseSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_releaseSource
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  cs::ReleaseSource(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    notifySourceFrame
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_notifySourceFrame
  (JNIEnv *env, jclass, jint source)
{
  CS_Status status;
  cs::NotifySourceFrame(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    notifySourceError
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_notifySourceError
  (JNIEnv *env, jclass, jint source, jstring msg)
{
  CS_Status status;
  cs::NotifySourceError(source, JavaStringRef{env, msg}, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSourceConnected
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSourceConnected
  (JNIEnv *env, jclass, jint source, jboolean connected)
{
  CS_Status status;
  cs::SetSourceConnected(source, connected, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createSourceProperty
 * Signature: (ILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createSourceProperty
  (JNIEnv *env, jclass, jint source, jstring name, jint type)
{
  CS_Status status;
  auto val =
      cs::CreateSourceProperty(source, JavaStringRef{env, name},
                               static_cast<CS_PropertyType>(type), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSourceProperty
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSourceProperty
  (JNIEnv *env, jclass, jint source, jint property)
{
  CS_Status status;
  cs::RemoveSourceProperty(source, property, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSourcePropertyByName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSourcePropertyByName
  (JNIEnv *env, jclass, jint source, jstring name)
{
  CS_Status status;
  cs::RemoveSourceProperty(source, JavaStringRef{env, name}, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createHTTPSink
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createHTTPSink
  (JNIEnv *env, jclass, jstring name, jstring listenAddress, jint port)
{
  CS_Status status;
  auto val =
      cs::CreateHTTPSink(JavaStringRef{env, name},
                         JavaStringRef{env, listenAddress}, port, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    createCvSink
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_createCvSink
  (JNIEnv *env, jclass, jstring name)
{
  CS_Status status;
  auto val = cs::CreateCvSink(JavaStringRef{env, name}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkName
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetSinkName(sink, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkDescription
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetSinkDescription(sink, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSinkSource
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSinkSource
  (JNIEnv *env, jclass, jint sink, jint source)
{
  CS_Status status;
  cs::SetSinkSource(sink, source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkSourceProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkSourceProperty
  (JNIEnv *env, jclass, jint sink, jstring name)
{
  CS_Status status;
  auto val = cs::GetSinkSourceProperty(sink, JavaStringRef{env, name}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkSource
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkSource
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  auto val = cs::GetSinkSource(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    copySink
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_copySink
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  auto val = cs::CopySink(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    releaseSink
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_releaseSink
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  cs::ReleaseSink(sink, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSinkSourceChannel
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSinkSourceChannel
  (JNIEnv *env, jclass, jint sink, jint channel)
{
  CS_Status status;
  cs::SetSinkSourceChannel(sink, channel, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    sinkWaitForFrame
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_sinkWaitForFrame
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  auto val = cs::SinkWaitForFrame(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    getSinkError
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_getSinkError
  (JNIEnv *env, jclass, jint sink)
{
  CS_Status status;
  llvm::SmallString<128> str;
  cs::GetSinkError(sink, str, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaString(env, str);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    setSinkEnabled
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_setSinkEnabled
  (JNIEnv *env, jclass, jint sink, jboolean enabled)
{
  CS_Status status;
  cs::SetSinkEnabled(sink, enabled, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSourceListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSourceListener
  (JNIEnv *env, jclass, jint handle)
{
  CS_Status status;
  cs::RemoveSourceListener(handle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    removeSinkListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_removeSinkListener
  (JNIEnv *env, jclass, jint handle)
{
  CS_Status status;
  cs::RemoveSinkListener(handle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateUSBCameras
 * Signature: ()[Ledu/wpi/cameraserver/USBCameraInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateUSBCameras
  (JNIEnv *env, jclass)
{
  CS_Status status;
  auto arr = cs::EnumerateUSBCameras(&status);
  if (!CheckStatus(env, status)) return nullptr;
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), usbCameraInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JavaLocal<jobject> jelem{env, ToJavaObject(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateSources
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateSources
  (JNIEnv *env, jclass)
{
  CS_Status status;
  llvm::SmallVector<CS_Source, 16> arr;
  cs::EnumerateSources(arr, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaIntArray(env, arr);
}

/*
 * Class:     edu_wpi_cameraserver_CameraServerJNI
 * Method:    enumerateSinks
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_edu_wpi_cameraserver_CameraServerJNI_enumerateSinks
  (JNIEnv *env, jclass)
{
  CS_Status status;
  llvm::SmallVector<CS_Sink, 16> arr;
  cs::EnumerateSinks(arr, &status);
  if (!CheckStatus(env, status)) return nullptr;
  return ToJavaIntArray(env, arr);
}

}  // extern "C"
