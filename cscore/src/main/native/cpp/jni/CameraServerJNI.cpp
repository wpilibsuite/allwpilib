// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <span>
#include <string>

#include <fmt/format.h>

#define WPI_RAWFRAME_JNI
#include <wpi/RawFrame.h>
#include <wpi/SmallString.h>
#include <wpi/jni_util.h>

#include "cscore_raw.h"
#include "cscore_runloop.h"
#include "edu_wpi_first_cscore_CameraServerJNI.h"

namespace cv {
class Mat;
}  // namespace cv

using namespace wpi::java;

//
// Globals and load/unload
//

// Used for callback.
static JavaVM* jvm = nullptr;
static JClass usbCameraInfoCls;
static JClass videoModeCls;
static JClass videoEventCls;
static JClass rawFrameCls;
static JException videoEx;
static JException interruptedEx;
static JException nullPointerEx;
static JException unsupportedEx;
static JException exceptionEx;
// Thread-attached environment for listener callbacks.
static JNIEnv* listenerEnv = nullptr;

static const JClassInit classes[] = {
    {"edu/wpi/first/cscore/UsbCameraInfo", &usbCameraInfoCls},
    {"edu/wpi/first/cscore/VideoMode", &videoModeCls},
    {"edu/wpi/first/cscore/VideoEvent", &videoEventCls},
    {"edu/wpi/first/util/RawFrame", &rawFrameCls}};

static const JExceptionInit exceptions[] = {
    {"edu/wpi/first/cscore/VideoException", &videoEx},
    {"java/lang/InterruptedException", &interruptedEx},
    {"java/lang/NullPointerException", &nullPointerEx},
    {"java/lang/UnsupportedOperationException", &unsupportedEx},
    {"java/lang/Exception", &exceptionEx}};

static void ListenerOnStart() {
  if (!jvm) {
    return;
  }
  JNIEnv* env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_2;
  args.name = const_cast<char*>("CSListener");
  args.group = nullptr;
  if (jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), &args) !=
      JNI_OK) {
    return;
  }
  if (!env || !env->functions) {
    return;
  }
  listenerEnv = env;
}

static void ListenerOnExit() {
  listenerEnv = nullptr;
  if (!jvm) {
    return;
  }
  jvm->DetachCurrentThread();
}

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  // Cache references to classes
  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  for (auto& c : exceptions) {
    *c.cls = JException(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  // Initial configuration of listener start/exit
  cs::SetListenerOnStart(ListenerOnStart);
  cs::SetListenerOnExit(ListenerOnExit);

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  cs::Shutdown();

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }
  // Delete global references
  for (auto& c : classes) {
    c.cls->free(env);
  }
  for (auto& c : exceptions) {
    c.cls->free(env);
  }
  jvm = nullptr;
}

}  // extern "C"

//
// Helper class to create and clean up a global reference
//
template <typename T>
class JCSGlobal {
 public:
  JCSGlobal(JNIEnv* env, T obj)
      : m_obj(static_cast<T>(env->NewGlobalRef(obj))) {}
  ~JCSGlobal() {
    if (!jvm || cs::NotifierDestroyed()) {
      return;
    }
    JNIEnv* env;
    bool attached = false;
    // don't attach and de-attach if already attached to a thread.
    if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) ==
        JNI_EDETACHED) {
      if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) !=
          JNI_OK) {
        return;
      }
      attached = true;
    }
    if (!env || !env->functions) {
      return;
    }
    env->DeleteGlobalRef(m_obj);
    if (attached) {
      jvm->DetachCurrentThread();
    }
  }
  operator T() { return m_obj; }  // NOLINT
  T obj() { return m_obj; }

 private:
  T m_obj;
};

static void ReportError(JNIEnv* env, CS_Status status) {
  if (status == CS_OK) {
    return;
  }
  std::string_view msg;
  std::string msgBuf;
  switch (status) {
    case CS_PROPERTY_WRITE_FAILED:
      msg = "property write failed";
      break;
    case CS_INVALID_HANDLE:
      msg = "invalid handle";
      break;
    case CS_WRONG_HANDLE_SUBTYPE:
      msg = "wrong handle subtype";
      break;
    case CS_INVALID_PROPERTY:
      msg = "invalid property";
      break;
    case CS_WRONG_PROPERTY_TYPE:
      msg = "wrong property type";
      break;
    case CS_READ_FAILED:
      msg = "read failed";
      break;
    case CS_SOURCE_IS_DISCONNECTED:
      msg = "source is disconnected";
      break;
    case CS_EMPTY_VALUE:
      msg = "empty value";
      break;
    case CS_BAD_URL:
      msg = "bad URL";
      break;
    case CS_TELEMETRY_NOT_ENABLED:
      msg = "telemetry not enabled";
      break;
    default: {
      msgBuf = fmt::format("unknown error code={}", status);
      msg = msgBuf;
      break;
    }
  }
  videoEx.Throw(env, msg);
}

static inline bool CheckStatus(JNIEnv* env, CS_Status status) {
  if (status != CS_OK) {
    ReportError(env, status);
  }
  return status == CS_OK;
}

static jobject MakeJObject(JNIEnv* env, const cs::UsbCameraInfo& info) {
  static jmethodID constructor = env->GetMethodID(
      usbCameraInfoCls, "<init>",
      "(ILjava/lang/String;Ljava/lang/String;[Ljava/lang/String;II)V");
  JLocal<jstring> path(env, MakeJString(env, info.path));
  JLocal<jstring> name(env, MakeJString(env, info.name));
  JLocal<jobjectArray> otherPaths(env, MakeJStringArray(env, info.otherPaths));
  return env->NewObject(usbCameraInfoCls, constructor,
                        static_cast<jint>(info.dev), path.obj(), name.obj(),
                        otherPaths.obj(), static_cast<jint>(info.vendorId),
                        static_cast<jint>(info.productId));
}

static jobject MakeJObject(JNIEnv* env, const cs::VideoMode& videoMode) {
  static jmethodID constructor =
      env->GetMethodID(videoModeCls, "<init>", "(IIII)V");
  return env->NewObject(
      videoModeCls, constructor, static_cast<jint>(videoMode.pixelFormat),
      static_cast<jint>(videoMode.width), static_cast<jint>(videoMode.height),
      static_cast<jint>(videoMode.fps));
}

static jobject MakeJObject(JNIEnv* env, const cs::RawEvent& event) {
  static jmethodID constructor =
      env->GetMethodID(videoEventCls, "<init>",
                       "(IIILjava/lang/String;IIIIIIILjava/lang/String;I)V");
  JLocal<jstring> name(env, MakeJString(env, event.name));
  JLocal<jstring> valueStr(env, MakeJString(env, event.valueStr));
  // clang-format off
  return env->NewObject(
      videoEventCls,
      constructor,
      static_cast<jint>(event.kind),
      static_cast<jint>(event.sourceHandle),
      static_cast<jint>(event.sinkHandle),
      name.obj(),
      static_cast<jint>(event.mode.pixelFormat),
      static_cast<jint>(event.mode.width),
      static_cast<jint>(event.mode.height),
      static_cast<jint>(event.mode.fps),
      static_cast<jint>(event.propertyHandle),
      static_cast<jint>(event.propertyKind),
      static_cast<jint>(event.value),
      valueStr.obj(),
      static_cast<jint>(event.listener));
  // clang-format on
}

static jobjectArray MakeJObject(JNIEnv* env,
                                std::span<const cs::RawEvent> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), videoEventCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

extern "C" {

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getPropertyKind
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getPropertyKind
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto val = cs::GetPropertyKind(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getPropertyName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getPropertyName
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetPropertyName(property, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getProperty
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getProperty
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto val = cs::GetProperty(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setProperty
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setProperty
  (JNIEnv* env, jclass, jint property, jint value)
{
  CS_Status status = 0;
  cs::SetProperty(property, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getPropertyMin
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getPropertyMin
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto val = cs::GetPropertyMin(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getPropertyMax
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getPropertyMax
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto val = cs::GetPropertyMax(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getPropertyStep
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getPropertyStep
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto val = cs::GetPropertyStep(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getPropertyDefault
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getPropertyDefault
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto val = cs::GetPropertyDefault(property, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getStringProperty
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getStringProperty
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetStringProperty(property, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setStringProperty
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setStringProperty
  (JNIEnv* env, jclass, jint property, jstring value)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  CS_Status status = 0;
  cs::SetStringProperty(property, JStringRef{env, value}.str(), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getEnumPropertyChoices
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getEnumPropertyChoices
  (JNIEnv* env, jclass, jint property)
{
  CS_Status status = 0;
  auto arr = cs::GetEnumPropertyChoices(property, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJStringArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createUsbCameraDev
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createUsbCameraDev
  (JNIEnv* env, jclass, jstring name, jint dev)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::CreateUsbCameraDev(JStringRef{env, name}.str(), dev, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createUsbCameraPath
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createUsbCameraPath
  (JNIEnv* env, jclass, jstring name, jstring path)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  if (!path) {
    nullPointerEx.Throw(env, "path cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::CreateUsbCameraPath(JStringRef{env, name}.str(),
                                     JStringRef{env, path}.str(), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createHttpCamera
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createHttpCamera
  (JNIEnv* env, jclass, jstring name, jstring url, jint kind)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  if (!url) {
    nullPointerEx.Throw(env, "url cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::CreateHttpCamera(
      JStringRef{env, name}.str(), JStringRef{env, url}.str(),
      static_cast<CS_HttpCameraKind>(kind), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createHttpCameraMulti
 * Signature: (Ljava/lang/String;[Ljava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createHttpCameraMulti
  (JNIEnv* env, jclass, jstring name, jobjectArray urls, jint kind)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  if (!urls) {
    nullPointerEx.Throw(env, "urls cannot be null");
    return 0;
  }
  size_t len = env->GetArrayLength(urls);
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(urls, i))};
    if (!elem) {
      // TODO
      return 0;
    }
    vec.emplace_back(JStringRef{env, elem}.str());
  }
  CS_Status status = 0;
  auto val =
      cs::CreateHttpCamera(JStringRef{env, name}.str(), vec,
                           static_cast<CS_HttpCameraKind>(kind), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createRawSource
 * Signature: (Ljava/lang/String;ZIIII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createRawSource
  (JNIEnv* env, jclass, jstring name, jboolean isCv, jint pixelFormat,
   jint width, jint height, jint fps)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::CreateRawSource(
      JStringRef{env, name}.str(), isCv,
      cs::VideoMode{static_cast<cs::VideoMode::PixelFormat>(pixelFormat),
                    static_cast<int>(width), static_cast<int>(height),
                    static_cast<int>(fps)},
      &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceKind
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceKind
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::GetSourceKind(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceName
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetSourceName(source, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceDescription
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetSourceDescription(source, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceLastFrameTime
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceLastFrameTime
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::GetSourceLastFrameTime(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceConnectionStrategy
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceConnectionStrategy
  (JNIEnv* env, jclass, jint source, jint strategy)
{
  CS_Status status = 0;
  cs::SetSourceConnectionStrategy(
      source, static_cast<CS_ConnectionStrategy>(strategy), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    isSourceConnected
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_isSourceConnected
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::IsSourceConnected(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    isSourceEnabled
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_isSourceEnabled
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::IsSourceEnabled(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceProperty
  (JNIEnv* env, jclass, jint source, jstring name)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val =
      cs::GetSourceProperty(source, JStringRef{env, name}.str(), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateSourceProperties
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateSourceProperties
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  wpi::SmallVector<CS_Property, 32> buf;
  auto arr = cs::EnumerateSourceProperties(source, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJIntArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceVideoMode
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceVideoMode
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::GetSourceVideoMode(source, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJObject(env, val);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceVideoMode
 * Signature: (IIIII)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceVideoMode
  (JNIEnv* env, jclass, jint source, jint pixelFormat, jint width, jint height,
   jint fps)
{
  CS_Status status = 0;
  auto val = cs::SetSourceVideoMode(
      source,
      cs::VideoMode(static_cast<cs::VideoMode::PixelFormat>(pixelFormat), width,
                    height, fps),
      &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourcePixelFormat
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourcePixelFormat
  (JNIEnv* env, jclass, jint source, jint pixelFormat)
{
  CS_Status status = 0;
  auto val = cs::SetSourcePixelFormat(
      source, static_cast<cs::VideoMode::PixelFormat>(pixelFormat), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceResolution
 * Signature: (III)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceResolution
  (JNIEnv* env, jclass, jint source, jint width, jint height)
{
  CS_Status status = 0;
  auto val = cs::SetSourceResolution(source, width, height, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceFPS
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceFPS
  (JNIEnv* env, jclass, jint source, jint fps)
{
  CS_Status status = 0;
  auto val = cs::SetSourceFPS(source, fps, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceConfigJson
 * Signature: (ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceConfigJson
  (JNIEnv* env, jclass, jint source, jstring config)
{
  CS_Status status = 0;
  auto val = cs::SetSourceConfigJson(source, JStringRef{env, config}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSourceConfigJson
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSourceConfigJson
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::GetSourceConfigJson(source, &status);
  CheckStatus(env, status);
  return MakeJString(env, val);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateSourceVideoModes
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateSourceVideoModes
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto arr = cs::EnumerateSourceVideoModes(source, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  jobjectArray jarr = env->NewObjectArray(arr.size(), videoModeCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> jelem{env, MakeJObject(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateSourceSinks
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateSourceSinks
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  wpi::SmallVector<CS_Sink, 16> buf;
  auto arr = cs::EnumerateSourceSinks(source, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJIntArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    copySource
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_copySource
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::CopySource(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    releaseSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_releaseSource
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  cs::ReleaseSource(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraBrightness
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraBrightness
  (JNIEnv* env, jclass, jint source, jint brightness)
{
  CS_Status status = 0;
  cs::SetCameraBrightness(source, brightness, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getCameraBrightness
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getCameraBrightness
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::GetCameraBrightness(source, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraWhiteBalanceAuto
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraWhiteBalanceAuto
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  cs::SetCameraWhiteBalanceAuto(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraWhiteBalanceHoldCurrent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraWhiteBalanceHoldCurrent
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  cs::SetCameraWhiteBalanceHoldCurrent(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraWhiteBalanceManual
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraWhiteBalanceManual
  (JNIEnv* env, jclass, jint source, jint value)
{
  CS_Status status = 0;
  cs::SetCameraWhiteBalanceManual(source, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraExposureAuto
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraExposureAuto
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  cs::SetCameraExposureAuto(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraExposureHoldCurrent
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraExposureHoldCurrent
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  cs::SetCameraExposureHoldCurrent(source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setCameraExposureManual
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setCameraExposureManual
  (JNIEnv* env, jclass, jint source, jint value)
{
  CS_Status status = 0;
  cs::SetCameraExposureManual(source, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setUsbCameraPath
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setUsbCameraPath
  (JNIEnv* env, jclass, jint source, jstring path)
{
  CS_Status status = 0;
  cs::SetUsbCameraPath(source, JStringRef{env, path}.str(), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getUsbCameraPath
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getUsbCameraPath
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto str = cs::GetUsbCameraPath(source, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getUsbCameraInfo
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getUsbCameraInfo
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto info = cs::GetUsbCameraInfo(source, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJObject(env, info);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getHttpCameraKind
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getHttpCameraKind
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto kind = cs::GetHttpCameraKind(source, &status);
  if (!CheckStatus(env, status)) {
    return 0;
  }
  return kind;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setHttpCameraUrls
 * Signature: (I[Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setHttpCameraUrls
  (JNIEnv* env, jclass, jint source, jobjectArray urls)
{
  if (!urls) {
    nullPointerEx.Throw(env, "urls cannot be null");
    return;
  }
  size_t len = env->GetArrayLength(urls);
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(urls, i))};
    if (!elem) {
      // TODO
      return;
    }
    vec.emplace_back(JStringRef{env, elem}.str());
  }
  CS_Status status = 0;
  cs::SetHttpCameraUrls(source, vec, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getHttpCameraUrls
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getHttpCameraUrls
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto arr = cs::GetHttpCameraUrls(source, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJStringArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    putRawSourceFrame
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_putRawSourceFrame
  (JNIEnv* env, jclass, jint source, jlong framePtr)
{
  auto* frame = reinterpret_cast<wpi::RawFrame*>(framePtr);
  if (!frame) {
    nullPointerEx.Throw(env, "frame is null");
    return;
  }
  CS_Status status = 0;
  cs::PutSourceFrame(source, *frame, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    putRawSourceFrameBB
 * Signature: (ILjava/lang/Object;IIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_putRawSourceFrameBB
  (JNIEnv* env, jclass, jint source, jobject data, jint size, jint width,
   jint height, jint stride, jint pixelFormat)
{
  WPI_RawFrame frame;  // use WPI_Frame because we don't want the destructor
  frame.data = static_cast<uint8_t*>(env->GetDirectBufferAddress(data));
  if (!frame.data) {
    nullPointerEx.Throw(env, "data is null");
    return;
  }
  frame.freeFunc = nullptr;
  frame.freeCbData = nullptr;
  frame.size = size;
  frame.width = width;
  frame.height = height;
  frame.stride = stride;
  frame.pixelFormat = pixelFormat;
  CS_Status status = 0;
  cs::PutSourceFrame(source, frame, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    putRawSourceFrameData
 * Signature: (IJIIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_putRawSourceFrameData
  (JNIEnv* env, jclass, jint source, jlong data, jint size, jint width,
   jint height, jint stride, jint pixelFormat)
{
  WPI_RawFrame frame;  // use WPI_Frame because we don't want the destructor
  frame.data = reinterpret_cast<uint8_t*>(data);
  if (!frame.data) {
    nullPointerEx.Throw(env, "data is null");
    return;
  }
  frame.freeFunc = nullptr;
  frame.freeCbData = nullptr;
  frame.size = size;
  frame.width = width;
  frame.height = height;
  frame.stride = stride;
  frame.pixelFormat = pixelFormat;
  CS_Status status = 0;
  cs::PutSourceFrame(source, frame, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    notifySourceError
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_notifySourceError
  (JNIEnv* env, jclass, jint source, jstring msg)
{
  if (!msg) {
    nullPointerEx.Throw(env, "msg cannot be null");
    return;
  }
  CS_Status status = 0;
  cs::NotifySourceError(source, JStringRef{env, msg}.str(), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceConnected
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceConnected
  (JNIEnv* env, jclass, jint source, jboolean connected)
{
  CS_Status status = 0;
  cs::SetSourceConnected(source, connected, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceDescription
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceDescription
  (JNIEnv* env, jclass, jint source, jstring description)
{
  if (!description) {
    nullPointerEx.Throw(env, "description cannot be null");
    return;
  }
  CS_Status status = 0;
  cs::SetSourceDescription(source, JStringRef{env, description}.str(), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createSourceProperty
 * Signature: (ILjava/lang/String;IIIIII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createSourceProperty
  (JNIEnv* env, jclass, jint source, jstring name, jint kind, jint minimum,
   jint maximum, jint step, jint defaultValue, jint value)
{
  CS_Status status = 0;
  auto val = cs::CreateSourceProperty(
      source, JStringRef{env, name}.str(), static_cast<CS_PropertyKind>(kind),
      minimum, maximum, step, defaultValue, value, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSourceEnumPropertyChoices
 * Signature: (II[Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSourceEnumPropertyChoices
  (JNIEnv* env, jclass, jint source, jint property, jobjectArray choices)
{
  if (!choices) {
    nullPointerEx.Throw(env, "choices cannot be null");
    return;
  }
  size_t len = env->GetArrayLength(choices);
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(choices, i))};
    if (!elem) {
      // TODO
      return;
    }
    vec.emplace_back(JStringRef{env, elem}.str());
  }
  CS_Status status = 0;
  cs::SetSourceEnumPropertyChoices(source, property, vec, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createMjpegServer
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createMjpegServer
  (JNIEnv* env, jclass, jstring name, jstring listenAddress, jint port)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  if (!listenAddress) {
    nullPointerEx.Throw(env, "listenAddress cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::CreateMjpegServer(JStringRef{env, name}.str(),
                                   JStringRef{env, listenAddress}.str(), port,
                                   &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createRawSink
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createRawSink
  (JNIEnv* env, jclass, jstring name, jboolean isCv)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::CreateRawSink(JStringRef{env, name}.str(), isCv, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkKind
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkKind
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  auto val = cs::GetSinkKind(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkName
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetSinkName(sink, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkDescription
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetSinkDescription(sink, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkProperty
  (JNIEnv* env, jclass, jint sink, jstring name)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val = cs::GetSinkProperty(sink, JStringRef{env, name}.str(), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateSinkProperties
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateSinkProperties
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  wpi::SmallVector<CS_Property, 32> buf;
  auto arr = cs::EnumerateSinkProperties(source, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJIntArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSinkConfigJson
 * Signature: (ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSinkConfigJson
  (JNIEnv* env, jclass, jint source, jstring config)
{
  CS_Status status = 0;
  auto val = cs::SetSinkConfigJson(source, JStringRef{env, config}, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkConfigJson
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkConfigJson
  (JNIEnv* env, jclass, jint source)
{
  CS_Status status = 0;
  auto val = cs::GetSinkConfigJson(source, &status);
  CheckStatus(env, status);
  return MakeJString(env, val);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSinkSource
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSinkSource
  (JNIEnv* env, jclass, jint sink, jint source)
{
  CS_Status status = 0;
  cs::SetSinkSource(sink, source, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkSourceProperty
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkSourceProperty
  (JNIEnv* env, jclass, jint sink, jstring name)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return 0;
  }
  CS_Status status = 0;
  auto val =
      cs::GetSinkSourceProperty(sink, JStringRef{env, name}.str(), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkSource
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkSource
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  auto val = cs::GetSinkSource(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    copySink
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_copySink
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  auto val = cs::CopySink(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    releaseSink
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_releaseSink
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  cs::ReleaseSink(sink, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getMjpegServerListenAddress
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getMjpegServerListenAddress
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  auto str = cs::GetMjpegServerListenAddress(sink, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getMjpegServerPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getMjpegServerPort
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  auto val = cs::GetMjpegServerPort(sink, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSinkDescription
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSinkDescription
  (JNIEnv* env, jclass, jint sink, jstring description)
{
  if (!description) {
    nullPointerEx.Throw(env, "description cannot be null");
    return;
  }
  CS_Status status = 0;
  cs::SetSinkDescription(sink, JStringRef{env, description}.str(), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    grabRawSinkFrame
 * Signature: (ILjava/lang/Object;J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_grabRawSinkFrame
  (JNIEnv* env, jclass, jint sink, jobject frameObj, jlong framePtr)
{
  auto* frame = reinterpret_cast<wpi::RawFrame*>(framePtr);
  auto origData = frame->data;
  CS_Status status = 0;
  auto rv = cs::GrabSinkFrame(static_cast<CS_Sink>(sink), *frame, &status);
  if (!CheckStatus(env, status)) {
    return 0;
  }
  wpi::SetFrameData(env, rawFrameCls, frameObj, *frame,
                    origData != frame->data);
  return rv;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    grabRawSinkFrameTimeout
 * Signature: (ILjava/lang/Object;JD)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_grabRawSinkFrameTimeout
  (JNIEnv* env, jclass, jint sink, jobject frameObj, jlong framePtr,
   jdouble timeout)
{
  auto* frame = reinterpret_cast<wpi::RawFrame*>(framePtr);
  auto origData = frame->data;
  CS_Status status = 0;
  auto rv = cs::GrabSinkFrameTimeout(static_cast<CS_Sink>(sink), *frame,
                                     timeout, &status);
  if (!CheckStatus(env, status)) {
    return 0;
  }
  wpi::SetFrameData(env, rawFrameCls, frameObj, *frame,
                    origData != frame->data);
  return rv;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getSinkError
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getSinkError
  (JNIEnv* env, jclass, jint sink)
{
  CS_Status status = 0;
  wpi::SmallString<128> buf;
  auto str = cs::GetSinkError(sink, buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJString(env, str);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setSinkEnabled
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setSinkEnabled
  (JNIEnv* env, jclass, jint sink, jboolean enabled)
{
  CS_Status status = 0;
  cs::SetSinkEnabled(sink, enabled, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    addListener
 * Signature: (Ljava/lang/Object;IZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_addListener
  (JNIEnv* envouter, jclass, jobject listener, jint eventMask,
   jboolean immediateNotify)
{
  if (!listener) {
    nullPointerEx.Throw(envouter, "listener cannot be null");
    return 0;
  }
  // the shared pointer to the weak global will keep it around until the
  // entry listener is destroyed
  auto listener_global =
      std::make_shared<JCSGlobal<jobject>>(envouter, listener);

  // cls is a temporary here; cannot be used within callback functor
  jclass cls = envouter->GetObjectClass(listener);
  if (!cls) {
    return 0;
  }

  // method ids, on the other hand, are safe to retain
  jmethodID mid = envouter->GetMethodID(cls, "accept", "(Ljava/lang/Object;)V");
  if (!mid) {
    return 0;
  }

  CS_Status status = 0;
  CS_Listener handle = cs::AddListener(
      [=](const cs::RawEvent& event) {
        JNIEnv* env = listenerEnv;
        if (!env || !env->functions) {
          return;
        }

        // get the handler
        auto handler = listener_global->obj();

        // convert into the appropriate Java type
        JLocal<jobject> jobj{env, MakeJObject(env, event)};
        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
          return;
        }
        if (!jobj) {
          return;
        }

        env->CallVoidMethod(handler, mid, jobj.obj());
        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
        }
      },
      eventMask, immediateNotify != JNI_FALSE, &status);
  CheckStatus(envouter, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    removeListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_removeListener
  (JNIEnv* env, jclass, jint handle)
{
  CS_Status status = 0;
  cs::RemoveListener(handle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    createListenerPoller
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_createListenerPoller
  (JNIEnv*, jclass)
{
  return cs::CreateListenerPoller();
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    destroyListenerPoller
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_destroyListenerPoller
  (JNIEnv*, jclass, jint poller)
{
  cs::DestroyListenerPoller(poller);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    addPolledListener
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_addPolledListener
  (JNIEnv* env, jclass, jint poller, jint eventMask, jboolean immediateNotify)
{
  CS_Status status = 0;
  auto rv = cs::AddPolledListener(poller, eventMask, immediateNotify, &status);
  CheckStatus(env, status);
  return rv;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    pollListener
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_pollListener
  (JNIEnv* env, jclass, jint poller)
{
  auto events = cs::PollListener(poller);
  if (events.empty()) {
    interruptedEx.Throw(env, "PollListener interrupted");
    return nullptr;
  }
  return MakeJObject(env, events);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    pollListenerTimeout
 * Signature: (ID)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_pollListenerTimeout
  (JNIEnv* env, jclass, jint poller, jdouble timeout)
{
  bool timed_out = false;
  auto events = cs::PollListener(poller, timeout, &timed_out);
  if (events.empty() && !timed_out) {
    interruptedEx.Throw(env, "PollListener interrupted");
    return nullptr;
  }
  return MakeJObject(env, events);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    cancelPollListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_cancelPollListener
  (JNIEnv*, jclass, jint poller)
{
  cs::CancelPollListener(poller);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setTelemetryPeriod
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setTelemetryPeriod
  (JNIEnv* env, jclass, jdouble seconds)
{
  cs::SetTelemetryPeriod(seconds);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getTelemetryElapsedTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getTelemetryElapsedTime
  (JNIEnv* env, jclass)
{
  return cs::GetTelemetryElapsedTime();
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getTelemetryValue
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getTelemetryValue
  (JNIEnv* env, jclass, jint handle, jint kind)
{
  CS_Status status = 0;
  auto val = cs::GetTelemetryValue(handle, static_cast<CS_TelemetryKind>(kind),
                                   &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getTelemetryAverageValue
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getTelemetryAverageValue
  (JNIEnv* env, jclass, jint handle, jint kind)
{
  CS_Status status = 0;
  auto val = cs::GetTelemetryAverageValue(
      handle, static_cast<CS_TelemetryKind>(kind), &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateUsbCameras
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateUsbCameras
  (JNIEnv* env, jclass)
{
  CS_Status status = 0;
  auto arr = cs::EnumerateUsbCameras(&status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), usbCameraInfoCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> jelem{env, MakeJObject(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateSources
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateSources
  (JNIEnv* env, jclass)
{
  CS_Status status = 0;
  wpi::SmallVector<CS_Source, 16> buf;
  auto arr = cs::EnumerateSourceHandles(buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJIntArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    enumerateSinks
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_enumerateSinks
  (JNIEnv* env, jclass)
{
  CS_Status status = 0;
  wpi::SmallVector<CS_Sink, 16> buf;
  auto arr = cs::EnumerateSinkHandles(buf, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  return MakeJIntArray(env, arr);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getHostname
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getHostname
  (JNIEnv* env, jclass)
{
  return MakeJString(env, cs::GetHostname());
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    getNetworkInterfaces
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_getNetworkInterfaces
  (JNIEnv* env, jclass)
{
  return MakeJStringArray(env, cs::GetNetworkInterfaces());
}

}  // extern "C"

namespace {

struct LogMessage {
 public:
  LogMessage(unsigned int level, const char* file, unsigned int line,
             const char* msg)
      : m_level(level), m_file(file), m_line(line), m_msg(msg) {}

  void CallJava(JNIEnv* env, jobject func, jmethodID mid) {
    JLocal<jstring> file{env, MakeJString(env, m_file)};
    JLocal<jstring> msg{env, MakeJString(env, m_msg)};
    env->CallVoidMethod(func, mid, static_cast<jint>(m_level), file.obj(),
                        static_cast<jint>(m_line), msg.obj());
  }

  static const char* GetName() { return "CSLogger"; }
  static JavaVM* GetJVM() { return jvm; }

 private:
  unsigned int m_level;
  const char* m_file;
  unsigned int m_line;
  std::string m_msg;
};

using LoggerJNI = JSingletonCallbackManager<LogMessage>;

}  // namespace

extern "C" {

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    setLogger
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_setLogger
  (JNIEnv* env, jclass, jobject func, jint minLevel)
{
  if (!func) {
    nullPointerEx.Throw(env, "func cannot be null");
    return;
  }
  // cls is a temporary here; cannot be used within callback functor
  jclass cls = env->GetObjectClass(func);
  if (!cls) {
    return;
  }

  // method ids, on the other hand, are safe to retain
  jmethodID mid = env->GetMethodID(cls, "apply",
                                   "(ILjava/lang/String;ILjava/lang/String;)V");
  if (!mid) {
    return;
  }

  auto& logger = LoggerJNI::GetInstance();
  logger.Start();
  logger.SetFunc(env, func, mid);

  cs::SetLogger(
      [](unsigned int level, const char* file, unsigned int line,
         const char* msg) {
        LoggerJNI::GetInstance().Send(level, file, line, msg);
      },
      minLevel);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    runMainRunLoop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_runMainRunLoop
  (JNIEnv*, jclass)
{
  cs::RunMainRunLoop();
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    runMainRunLoopTimeout
 * Signature: (D)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_runMainRunLoopTimeout
  (JNIEnv*, jclass, jdouble timeoutSeconds)
{
  return cs::RunMainRunLoopTimeout(timeoutSeconds);
}

/*
 * Class:     edu_wpi_first_cscore_CameraServerJNI
 * Method:    stopMainRunLoop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_cscore_CameraServerJNI_stopMainRunLoop
  (JNIEnv*, jclass)
{
  return cs::StopMainRunLoop();
}

}  // extern "C"
