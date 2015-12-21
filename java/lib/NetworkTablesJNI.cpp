#include <jni.h>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <sstream>
#include <queue>
#include <thread>

#include "edu_wpi_first_wpilibj_networktables_NetworkTablesJNI.h"
#include "ntcore.h"
#include "atomic_static.h"

//
// Globals and load/unload
//

// Used for callback.
static JavaVM *jvm = nullptr;
static jclass booleanCls = nullptr;
static jclass doubleCls = nullptr;
static jclass stringCls = nullptr;
static jclass connectionInfoCls = nullptr;
static jclass entryInfoCls = nullptr;
static jclass keyNotDefinedEx = nullptr;
static jclass persistentEx = nullptr;
// Thread-attached environment for listener callbacks.
static JNIEnv *listenerEnv = nullptr;

static void ListenerOnStart() {
  if (!jvm) return;
  JNIEnv *env;
  if (jvm->AttachCurrentThread(reinterpret_cast<void **>(&env),
                               nullptr) != JNI_OK)
    return;
  if (!env || !env->functions) return;
  listenerEnv = env;
}

static void ListenerOnExit() {
  listenerEnv = nullptr;
  if (!jvm) return;
  jvm->DetachCurrentThread();
}

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  jvm = vm;

  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  // Cache references to classes
  jclass local;

  local = env->FindClass("java/lang/Boolean");
  if (!local) return JNI_ERR;
  booleanCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!booleanCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("java/lang/Double");
  if (!local) return JNI_ERR;
  doubleCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!doubleCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("java/lang/String");
  if (!local) return JNI_ERR;
  stringCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!stringCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("edu/wpi/first/wpilibj/networktables/ConnectionInfo");
  if (!local) return JNI_ERR;
  connectionInfoCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!connectionInfoCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local = env->FindClass("edu/wpi/first/wpilibj/networktables/EntryInfo");
  if (!local) return JNI_ERR;
  entryInfoCls = static_cast<jclass>(env->NewGlobalRef(local));
  if (!entryInfoCls) return JNI_ERR;
  env->DeleteLocalRef(local);

  local =
      env->FindClass("edu/wpi/first/wpilibj/networktables/NetworkTableKeyNotDefined");
  keyNotDefinedEx = static_cast<jclass>(env->NewGlobalRef(local));
  if (!keyNotDefinedEx) return JNI_ERR;
  env->DeleteLocalRef(local);

  local =
      env->FindClass("edu/wpi/first/wpilibj/networktables/PersistentException");
  persistentEx = static_cast<jclass>(env->NewGlobalRef(local));
  if (!persistentEx) return JNI_ERR;
  env->DeleteLocalRef(local);

  // Initial configuration of listener start/exit
  nt::SetListenerOnStart(ListenerOnStart);
  nt::SetListenerOnExit(ListenerOnExit);

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;
  // Delete global references
  if (booleanCls) env->DeleteGlobalRef(booleanCls);
  if (doubleCls) env->DeleteGlobalRef(doubleCls);
  if (stringCls) env->DeleteGlobalRef(stringCls);
  if (connectionInfoCls) env->DeleteGlobalRef(connectionInfoCls);
  if (entryInfoCls) env->DeleteGlobalRef(entryInfoCls);
  if (keyNotDefinedEx) env->DeleteGlobalRef(keyNotDefinedEx);
  if (persistentEx) env->DeleteGlobalRef(persistentEx);
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
// Helper class to create and clean up a global reference
//
template <typename T>
class JavaGlobal {
 public:
  JavaGlobal(JNIEnv *env, T obj)
      : m_obj(static_cast<T>(env->NewGlobalRef(obj))) {}
  ~JavaGlobal() {
    if (!jvm || nt::NotifierDestroyed()) return;
    JNIEnv *env;
    if (jvm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr) !=
        JNI_OK)
      return;
    if (!env || !env->functions) return;
    env->DeleteGlobalRef(m_obj);
    jvm->DetachCurrentThread();
  }
  operator T() { return m_obj; }
  T obj() { return m_obj; }

 private:
  T m_obj;
};

//
// Helper class to create and clean up a weak global reference
//
template <typename T>
class JavaWeakGlobal {
 public:
  JavaWeakGlobal(JNIEnv *env, T obj)
      : m_obj(static_cast<T>(env->NewWeakGlobalRef(obj))) {}
  ~JavaWeakGlobal() {
    if (!jvm || nt::NotifierDestroyed()) return;
    JNIEnv *env;
    if (jvm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr) !=
        JNI_OK)
      return;
    if (!env || !env->functions) return;
    env->DeleteWeakGlobalRef(m_obj);
    jvm->DetachCurrentThread();
  }
  JavaLocal<T> obj(JNIEnv *env) {
    return JavaLocal<T>(env, env->NewLocalRef(m_obj));
  }

 private:
  T m_obj;
};

//
// Conversions from Java objects to C++
//

class JavaStringRef {
 public:
  JavaStringRef(JNIEnv *env, jstring str)
      : m_env(env),
        m_jstr(str),
        m_str(env->GetStringUTFChars(str, nullptr)) {}
  ~JavaStringRef() { m_env->ReleaseStringUTFChars(m_jstr, m_str); }

  operator nt::StringRef() const { return nt::StringRef(m_str); }
  nt::StringRef str() const { return nt::StringRef(m_str); }
  const char *c_str() const { return m_str; }

 private:
  JNIEnv *m_env;
  jstring m_jstr;
  const char *m_str;
};

class JavaByteRef {
 public:
  JavaByteRef(JNIEnv *env, jbyteArray jarr)
      : m_env(env),
        m_jarr(jarr),
        m_elements(env->GetByteArrayElements(jarr, nullptr)),
        m_size(env->GetArrayLength(jarr)) {}
  ~JavaByteRef() {
    m_env->ReleaseByteArrayElements(m_jarr, m_elements, JNI_ABORT);
  }

  operator nt::StringRef() const {
    return nt::StringRef(reinterpret_cast<char *>(m_elements), m_size);
  }

 private:
  JNIEnv *m_env;
  jbyteArray m_jarr;
  jbyte *m_elements;
  size_t m_size;
};

class JavaByteRefBB {
 public:
  JavaByteRefBB(JNIEnv *env, jobject bb, int len)
      : m_elements(env->GetDirectBufferAddress(bb)), m_size(len) {}

  operator nt::StringRef() const {
    return nt::StringRef(reinterpret_cast<char *>(m_elements), m_size);
  }

 private:
  void *m_elements;
  size_t m_size;
};

std::shared_ptr<nt::Value> FromJavaRaw(JNIEnv *env, jbyteArray jarr) {
  size_t len = env->GetArrayLength(jarr);
  jbyte *elements =
      static_cast<jbyte *>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) return nullptr;
  auto rv = nt::Value::MakeRaw(
      nt::StringRef(reinterpret_cast<char *>(elements), len));
  env->ReleasePrimitiveArrayCritical(jarr, elements, JNI_ABORT);
  return rv;
}

std::shared_ptr<nt::Value> FromJavaRawBB(JNIEnv *env, jobject jbb, int len) {
  void* elements = env->GetDirectBufferAddress(jbb);
  if (!elements) return nullptr;
  auto rv = nt::Value::MakeRaw(
      nt::StringRef(reinterpret_cast<char *>(elements), len));
  return rv;
}

std::shared_ptr<nt::Value> FromJavaRpc(JNIEnv *env, jbyteArray jarr) {
  size_t len = env->GetArrayLength(jarr);
  jbyte *elements =
      static_cast<jbyte *>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) return nullptr;
  auto rv = nt::Value::MakeRpc(
      nt::StringRef(reinterpret_cast<char *>(elements), len));
  env->ReleasePrimitiveArrayCritical(jarr, elements, JNI_ABORT);
  return rv;
}

std::shared_ptr<nt::Value> FromJavaBooleanArray(JNIEnv *env,
                                                jbooleanArray jarr) {
  size_t len = env->GetArrayLength(jarr);
  std::vector<int> arr;
  arr.reserve(len);
  jboolean *elements =
      static_cast<jboolean*>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) return nullptr;
  for (size_t i = 0; i < len; ++i) arr.push_back(elements[i]);
  env->ReleasePrimitiveArrayCritical(jarr, elements, JNI_ABORT);
  return nt::Value::MakeBooleanArray(arr);
}

std::shared_ptr<nt::Value> FromJavaDoubleArray(JNIEnv *env, jdoubleArray jarr) {
  size_t len = env->GetArrayLength(jarr);
  jdouble *elements =
      static_cast<jdouble *>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) return nullptr;
  auto rv = nt::Value::MakeDoubleArray(nt::ArrayRef<double>(elements, len));
  env->ReleasePrimitiveArrayCritical(jarr, elements, JNI_ABORT);
  return rv;
}

std::shared_ptr<nt::Value> FromJavaStringArray(JNIEnv *env, jobjectArray jarr) {
  size_t len = env->GetArrayLength(jarr);
  std::vector<std::string> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JavaLocal<jstring> elem(
        env, static_cast<jstring>(env->GetObjectArrayElement(jarr, i)));
    if (!elem) return nullptr;
    arr.push_back(JavaStringRef(env, elem).str());
  }
  return nt::Value::MakeStringArray(std::move(arr));
}

//
// Conversions from C++ to Java objects
//

static inline jstring ToJavaString(JNIEnv *env, nt::StringRef str) {
  // fastpath if string already null terminated; if not, need to make a copy
  if (str.data()[str.size()] == '\0')
    return env->NewStringUTF(str.data());
  else
    return env->NewStringUTF(str.str().c_str());
}

static jbyteArray ToJavaByteArray(JNIEnv *env, nt::StringRef str) {
  jbyteArray jarr = env->NewByteArray(str.size());
  if (!jarr) return nullptr;
  env->SetByteArrayRegion(jarr, 0, str.size(),
                          reinterpret_cast<const jbyte *>(str.data()));
  return jarr;
}

static jbooleanArray ToJavaBooleanArray(JNIEnv *env, nt::ArrayRef<int> arr)
{
  jbooleanArray jarr = env->NewBooleanArray(arr.size());
  if (!jarr) return nullptr;
  jboolean *elements =
      static_cast<jboolean*>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i)
    elements[i] = arr[i];
  env->ReleasePrimitiveArrayCritical(jarr, elements, 0);
  return jarr;
}

static jdoubleArray ToJavaDoubleArray(JNIEnv *env, nt::ArrayRef<double> arr)
{
  jdoubleArray jarr = env->NewDoubleArray(arr.size());
  if (!jarr) return nullptr;
  env->SetDoubleArrayRegion(jarr, 0, arr.size(), arr.data());
  return jarr;
}

static jobjectArray ToJavaStringArray(JNIEnv *env,
                                      nt::ArrayRef<std::string> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), stringCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JavaLocal<jstring> elem(env, env->NewStringUTF(arr[i].c_str()));
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobject ToJavaObject(JNIEnv *env, const nt::Value& value) {
  static jmethodID booleanConstructor = nullptr;
  static jmethodID doubleConstructor = nullptr;
  if (!booleanConstructor)
    booleanConstructor = env->GetMethodID(booleanCls, "<init>", "(Z)V");
  if (!doubleConstructor)
    doubleConstructor = env->GetMethodID(doubleCls, "<init>", "(D)V");

  switch (value.type()) {
    case NT_BOOLEAN:
      return env->NewObject(booleanCls, booleanConstructor,
                            (jboolean)(value.GetBoolean() ? 1 : 0));
    case NT_DOUBLE:
      return env->NewObject(doubleCls, doubleConstructor,
                            (jdouble)value.GetDouble());
    case NT_STRING:
      return ToJavaString(env, value.GetString());
    case NT_RAW:
      return ToJavaByteArray(env, value.GetRaw());
    case NT_BOOLEAN_ARRAY:
      return ToJavaBooleanArray(env, value.GetBooleanArray());
    case NT_DOUBLE_ARRAY:
      return ToJavaDoubleArray(env, value.GetDoubleArray());
    case NT_STRING_ARRAY:
      return ToJavaStringArray(env, value.GetStringArray());
    case NT_RPC:
      return ToJavaByteArray(env, value.GetRpc());
    default:
      return nullptr;
  }
}

static jobject ToJavaObject(JNIEnv *env, const nt::ConnectionInfo &info) {
  static jmethodID constructor =
      env->GetMethodID(connectionInfoCls, "<init>",
                       "(Ljava/lang/String;Ljava/lang/String;IJI)V");
  JavaLocal<jstring> remote_id(env, ToJavaString(env, info.remote_id));
  JavaLocal<jstring> remote_name(env, ToJavaString(env, info.remote_name));
  return env->NewObject(connectionInfoCls, constructor, remote_id.obj(),
                        remote_name.obj(), (jint)info.remote_port,
                        (jlong)info.last_update, (jint)info.protocol_version);
}

static jobject ToJavaObject(JNIEnv *env, const nt::EntryInfo &info) {
  static jmethodID constructor =
      env->GetMethodID(entryInfoCls, "<init>", "(Ljava/lang/String;IIJ)V");
  JavaLocal<jstring> name(env, ToJavaString(env, info.name));
  return env->NewObject(entryInfoCls, constructor, name.obj(), (jint)info.type,
                        (jint)info.flags, (jlong)info.last_change);
}

//
// Exception throwers
//

static void ThrowTableKeyNotDefined(JNIEnv *env, jstring key) {
  static jmethodID constructor = nullptr;
  if (!constructor)
    constructor =
        env->GetMethodID(keyNotDefinedEx, "<init>", "(Ljava/lang/String;)V");
  jobject exception = env->NewObject(keyNotDefinedEx, constructor, key);
  env->Throw(static_cast<jthrowable>(exception));
}

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    containsKey
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_containsKey
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val) return false;
  return true;
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getType
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getType
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val) return NT_UNASSIGNED;
  return val->type();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putBoolean
 * Signature: (Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putBoolean
  (JNIEnv *env, jclass, jstring key, jboolean value)
{
  return nt::SetEntryValue(JavaStringRef(env, key),
                           nt::Value::MakeBoolean(value != JNI_FALSE));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putDouble
 * Signature: (Ljava/lang/String;D)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putDouble
  (JNIEnv *env, jclass, jstring key, jdouble value)
{
  return nt::SetEntryValue(JavaStringRef(env, key),
                           nt::Value::MakeDouble(value));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putString
  (JNIEnv *env, jclass, jstring key, jstring value)
{
  return nt::SetEntryValue(JavaStringRef(env, key),
                           nt::Value::MakeString(JavaStringRef(env, value)));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putRaw
 * Signature: (Ljava/lang/String;[B)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putRaw__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray value)
{
  auto v = FromJavaRaw(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putRaw
 * Signature: (Ljava/lang/String;Ljava/nio/ByteBuffer;I)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putRaw__Ljava_lang_String_2Ljava_nio_ByteBuffer_2I
  (JNIEnv *env, jclass, jstring key, jobject value, jint len)
{
  auto v = FromJavaRawBB(env, value, len);
  if (!v) return false;
  return nt::SetEntryValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putBooleanArray
 * Signature: (Ljava/lang/String;[Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putBooleanArray
  (JNIEnv *env, jclass, jstring key, jbooleanArray value)
{
  auto v = FromJavaBooleanArray(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putDoubleArray
 * Signature: (Ljava/lang/String;[D)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putDoubleArray
  (JNIEnv *env, jclass, jstring key, jdoubleArray value)
{
  auto v = FromJavaDoubleArray(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putStringArray
  (JNIEnv *env, jclass, jstring key, jobjectArray value)
{
  auto v = FromJavaStringArray(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutBoolean
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutBoolean
  (JNIEnv *env, jclass, jstring key, jboolean value)
{
  nt::SetEntryTypeValue(JavaStringRef(env, key), nt::Value::MakeBoolean(value != JNI_FALSE));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutDouble
 * Signature: (Ljava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutDouble
  (JNIEnv *env, jclass, jstring key, jdouble value)
{
  nt::SetEntryTypeValue(JavaStringRef(env, key), nt::Value::MakeDouble(value));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutString
  (JNIEnv *env, jclass, jstring key, jstring value)
{
  nt::SetEntryTypeValue(JavaStringRef(env, key),
                        nt::Value::MakeString(JavaStringRef(env, value)));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutRaw
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutRaw__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray value)
{
  auto v = FromJavaRaw(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutRaw
 * Signature: (Ljava/lang/String;Ljava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutRaw__Ljava_lang_String_2Ljava_nio_ByteBuffer_2I
  (JNIEnv *env, jclass, jstring key, jobject value, jint len)
{
  auto v = FromJavaRawBB(env, value, len);
  if (!v) return;
  nt::SetEntryTypeValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutBooleanArray
 * Signature: (Ljava/lang/String;[Z)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutBooleanArray
  (JNIEnv *env, jclass, jstring key, jbooleanArray value)
{
  auto v = FromJavaBooleanArray(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutDoubleArray
 * Signature: (Ljava/lang/String;[D)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutDoubleArray
  (JNIEnv *env, jclass, jstring key, jdoubleArray value)
{
  auto v = FromJavaDoubleArray(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutStringArray
  (JNIEnv *env, jclass, jstring key, jobjectArray value)
{
  auto v = FromJavaStringArray(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JavaStringRef(env, key), v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getValue
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getValue__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaObject(env, *val);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBoolean
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBoolean__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsBoolean()) {
    ThrowTableKeyNotDefined(env, key);
    return false;
  }
  return val->GetBoolean();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getDouble
 * Signature: (Ljava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getDouble__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsDouble()) {
    ThrowTableKeyNotDefined(env, key);
    return 0;
  }
  return val->GetDouble();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getString
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getString__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsString()) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaString(env, val->GetString());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRaw
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRaw__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsRaw()) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaByteArray(env, val->GetRaw());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBooleanArray
 * Signature: (Ljava/lang/String;)[Z
 */
JNIEXPORT jbooleanArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBooleanArray__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsBooleanArray()) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaBooleanArray(env, val->GetBooleanArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getDoubleArray
 * Signature: (Ljava/lang/String;)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getDoubleArray__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsDoubleArray()) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaDoubleArray(env, val->GetDoubleArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getStringArray
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getStringArray__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsStringArray()) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaStringArray(env, val->GetStringArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getValue
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getValue__Ljava_lang_String_2Ljava_lang_Object_2
  (JNIEnv *env, jclass, jstring key, jobject defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val) return defaultValue;
  return ToJavaObject(env, *val);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBoolean
 * Signature: (Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBoolean__Ljava_lang_String_2Z
  (JNIEnv *env, jclass, jstring key, jboolean defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsBoolean()) return defaultValue;
  return val->GetBoolean();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getDouble
 * Signature: (Ljava/lang/String;D)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getDouble__Ljava_lang_String_2D
  (JNIEnv *env, jclass, jstring key, jdouble defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsDouble()) return defaultValue;
  return val->GetDouble();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getString__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key, jstring defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsString()) return defaultValue;
  return ToJavaString(env, val->GetString());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRaw
 * Signature: (Ljava/lang/String;[B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRaw__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsRaw()) return defaultValue;
  return ToJavaByteArray(env, val->GetRaw());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBooleanArray
 * Signature: (Ljava/lang/String;[Z)[Z
 */
JNIEXPORT jbooleanArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBooleanArray__Ljava_lang_String_2_3Z
  (JNIEnv *env, jclass, jstring key, jbooleanArray defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsBooleanArray()) return defaultValue;
  return ToJavaBooleanArray(env, val->GetBooleanArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getDoubleArray
 * Signature: (Ljava/lang/String;[D)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getDoubleArray__Ljava_lang_String_2_3D
  (JNIEnv *env, jclass, jstring key, jdoubleArray defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsDoubleArray()) return defaultValue;
  return ToJavaDoubleArray(env, val->GetDoubleArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getStringArray__Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key, jobjectArray defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsStringArray()) return defaultValue;
  return ToJavaStringArray(env, val->GetStringArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setEntryFlags
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setEntryFlags
  (JNIEnv *env, jclass, jstring key, jint flags)
{
  nt::SetEntryFlags(JavaStringRef(env, key), flags);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getEntryFlags
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getEntryFlags
  (JNIEnv *env, jclass, jstring key)
{
  return nt::GetEntryFlags(JavaStringRef(env, key));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    deleteEntry
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_deleteEntry
  (JNIEnv *env, jclass, jstring key)
{
  nt::DeleteEntry(JavaStringRef(env, key));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    deleteAllEntries
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_deleteAllEntries
  (JNIEnv *, jclass)
{
  nt::DeleteAllEntries();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getEntries
 * Signature: (Ljava/lang/String;I)[Ledu/wpi/first/wpilibj/networktables/EntryInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getEntries
  (JNIEnv *env, jclass, jstring prefix, jint types)
{
  auto arr = nt::GetEntryInfo(JavaStringRef(env, prefix), types);
  jobjectArray jarr = env->NewObjectArray(arr.size(), entryInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JavaLocal<jobject> jelem(env, ToJavaObject(env, arr[i]));
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    flush
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_flush
  (JNIEnv *, jclass)
{
  nt::Flush();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    addEntryListener
 * Signature: (Ljava/lang/String;Ledu/wpi/first/wpilibj/networktables/NetworkTablesJNI/EntryListenerFunction;Z)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_addEntryListener
  (JNIEnv *envouter, jclass, jstring prefix, jobject listener, jint flags)
{
  // the shared pointer to the weak global will keep it around until the
  // entry listener is destroyed
  auto listener_global =
      std::make_shared<JavaGlobal<jobject>>(envouter, listener);

  // cls is a temporary here; cannot be used within callback functor
	jclass cls = envouter->GetObjectClass(listener);
  if (!cls) return 0;

  // method ids, on the other hand, are safe to retain
  jmethodID mid = envouter->GetMethodID(
      cls, "apply", "(ILjava/lang/String;Ljava/lang/Object;I)V");
  if (!mid) return 0;

  return nt::AddEntryListener(
      JavaStringRef(envouter, prefix),
      [=](unsigned int uid, nt::StringRef name,
          std::shared_ptr<nt::Value> value, unsigned int flags_) {
        JNIEnv *env = listenerEnv;
        if (!env || !env->functions) return;

        // get the handler
        auto handler = listener_global->obj();

        // convert the value into the appropriate Java type
        jobject jobj = ToJavaObject(env, *value);
        if (!jobj) return;

        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
          return;
        }
        env->CallVoidMethod(handler, mid, (jint)uid, ToJavaString(env, name),
                            jobj, (jint)(flags_));
        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
        }
      },
      flags);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    removeEntryListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_removeEntryListener
  (JNIEnv *, jclass, jint entryListenerUid)
{
  nt::RemoveEntryListener(entryListenerUid);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    addConnectionListener
 * Signature: (Ledu/wpi/first/wpilibj/networktables/NetworkTablesJNI/ConnectionListenerFunction;Z)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_addConnectionListener
  (JNIEnv *envouter, jclass, jobject listener, jboolean immediateNotify)
{
  // the shared pointer to the weak global will keep it around until the
  // entry listener is destroyed
  auto listener_global =
      std::make_shared<JavaGlobal<jobject>>(envouter, listener);

  // cls is a temporary here; cannot be used within callback functor
	jclass cls = envouter->GetObjectClass(listener);
  if (!cls) return 0;

  // method ids, on the other hand, are safe to retain
  jmethodID mid = envouter->GetMethodID(
      cls, "apply", "(IZLedu/wpi/first/wpilibj/networktables/ConnectionInfo;)V");
  if (!mid) return 0;

  return nt::AddConnectionListener(
      [=](unsigned int uid, bool connected, const nt::ConnectionInfo& conn) {
        JNIEnv *env = listenerEnv;
        if (!env || !env->functions) return;

        // get the handler
        auto handler = listener_global->obj();
        //if (!handler) goto done; // can happen due to weak reference

        // convert into the appropriate Java type
        jobject jobj = ToJavaObject(env, conn);
        if (!jobj) return;

        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
          return;
        }
        env->CallVoidMethod(handler, mid, (jint)uid,
                            (jboolean)(connected ? 1 : 0), jobj);
        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
        }
      },
      immediateNotify != JNI_FALSE);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    removeConnectionListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_removeConnectionListener
  (JNIEnv *, jclass, jint connListenerUid)
{
  nt::RemoveConnectionListener(connListenerUid);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRpc
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRpc__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsRpc()) {
    ThrowTableKeyNotDefined(env, key);
    return nullptr;
  }
  return ToJavaByteArray(env, val->GetRpc());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRpc
 * Signature: (Ljava/lang/String;[B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRpc__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray defaultValue)
{
  auto val = nt::GetEntryValue(JavaStringRef(env, key));
  if (!val || !val->IsRpc()) return defaultValue;
  return ToJavaByteArray(env, val->GetRpc());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    callRpc
 * Signature: (Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_callRpc__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray params)
{
  return nt::CallRpc(JavaStringRef(env, key), JavaByteRef(env, params));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    callRpc
 * Signature: (Ljava/lang/String;Ljava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_callRpc__Ljava_lang_String_2Ljava_nio_ByteBuffer_2I
  (JNIEnv *env, jclass, jstring key, jobject params, jint params_len)
{
  return nt::CallRpc(JavaStringRef(env, key),
                     JavaByteRefBB(env, params, params_len));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setNetworkIdentity
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setNetworkIdentity
  (JNIEnv *env, jclass, jstring name)
{
  nt::SetNetworkIdentity(JavaStringRef(env, name));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    startServer
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_startServer
  (JNIEnv *env, jclass, jstring persistFilename, jstring listenAddress,
   jint port)
{
  nt::StartServer(JavaStringRef(env, persistFilename),
                  JavaStringRef(env, listenAddress).c_str(), port);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    stopServer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_stopServer
  (JNIEnv *, jclass)
{
  nt::StopServer();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_startClient
  (JNIEnv *env, jclass, jstring serverName, jint port)
{
  nt::StartClient(JavaStringRef(env, serverName).c_str(), port);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    stopClient
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_stopClient
  (JNIEnv *, jclass)
{
  nt::StopClient();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setUpdateRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setUpdateRate
  (JNIEnv *, jclass, jdouble interval)
{
  nt::SetUpdateRate(interval);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getConnections
 * Signature: ()[Ledu/wpi/first/wpilibj/networktables/ConnectionInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getConnections
  (JNIEnv *env, jclass)
{
  auto arr = nt::GetConnections();
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), connectionInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JavaLocal<jobject> jelem(env, ToJavaObject(env, arr[i]));
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    savePersistent
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_savePersistent
  (JNIEnv *env, jclass, jstring filename)
{
  const char *err = nt::SavePersistent(JavaStringRef(env, filename));
  if (err) env->ThrowNew(persistentEx, err);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    loadPersistent
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_loadPersistent
  (JNIEnv *env, jclass, jstring filename)
{
  std::vector<std::string> warns;
  const char *err = nt::LoadPersistent(JavaStringRef(env, filename),
                                       [&](size_t line, const char *msg) {
                                         std::ostringstream oss;
                                         oss << line << ": " << msg;
                                         warns.push_back(oss.str());
                                       });
  if (err) {
    env->ThrowNew(persistentEx, err);
    return nullptr;
  }
  return ToJavaStringArray(env, warns);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_now
  (JNIEnv *, jclass)
{
  return nt::Now();
}

}  // extern "C"

// Thread where log callbacks are actually performed.
//
// JNI's AttachCurrentThread() creates a Java Thread object on every
// invocation, which is both time inefficient and causes issues with Eclipse
// (which tries to keep a thread list up-to-date and thus gets swamped).
//
// Instead, this class attaches just once.  When a hardware notification
// occurs, a condition variable wakes up this thread and this thread actually
// makes the call into Java.
class LoggerThreadJNI {
 public:
  static LoggerThreadJNI& GetInstance() {
    ATOMIC_STATIC(LoggerThreadJNI, instance);
    return instance;
  }
  LoggerThreadJNI();
  ~LoggerThreadJNI();
  void SetFunc(JNIEnv* env, jobject func, jmethodID mid);
  void Start();
  void Stop();

  void Log(unsigned int level, const char* file, unsigned int line,
           const char* msg);

 private:
  void ThreadMain();

  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cond;
  std::atomic_bool m_active;
  struct LogMessage {
    LogMessage(unsigned int level_, const char* file_, unsigned int line_,
               const char* msg_)
        : level(level_), file(file_), line(line_), msg(msg_) {}
    unsigned int level;
    const char* file;
    unsigned int line;
    std::string msg;
  };
  std::queue<LogMessage> m_queue;
  std::mutex m_shutdown_mutex;
  std::condition_variable m_shutdown_cv;
  bool m_shutdown = false;
  jobject m_func = nullptr;
  jmethodID m_mid;

  ATOMIC_STATIC_DECL(LoggerThreadJNI)
};

ATOMIC_STATIC_INIT(LoggerThreadJNI)

LoggerThreadJNI::LoggerThreadJNI() {
  m_active = false;
}

LoggerThreadJNI::~LoggerThreadJNI() {
  Stop();
}

void LoggerThreadJNI::SetFunc(JNIEnv* env, jobject func, jmethodID mid) {
  std::lock_guard<std::mutex> lock(m_mutex);
  // free global reference
  if (m_func) env->DeleteGlobalRef(m_func);
  // create global reference
  m_func = env->NewGlobalRef(func);
  m_mid = mid;
}

void LoggerThreadJNI::Start() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active) return;
    m_active = true;
  }
  {
    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    m_shutdown = false;
  }
  m_thread = std::thread(&LoggerThreadJNI::ThreadMain, this);
}

void LoggerThreadJNI::Stop() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_active) return;
    m_active = false;
  }
  m_cond.notify_one();  // wake up thread

  // join threads, with timeout
  if (m_thread.joinable()) {
    std::unique_lock<std::mutex> lock(m_shutdown_mutex);
    auto timeout_time =
        std::chrono::steady_clock::now() + std::chrono::seconds(1);
    if (m_shutdown_cv.wait_until(lock, timeout_time,
                                 [&] { return m_shutdown; }))
      m_thread.join();
    else
      m_thread.detach();  // timed out, detach it
  }
}

void LoggerThreadJNI::Log(unsigned int level, const char *file,
                          unsigned int line, const char *msg) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_active) return;
  m_queue.emplace(level, file, line, msg);
  m_cond.notify_one();
}

void LoggerThreadJNI::ThreadMain() {
  JNIEnv *env;
  jint rs = jvm->AttachCurrentThread((void**)&env, NULL);
  if (rs != JNI_OK) return;

  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    m_cond.wait(lock, [&] { return !m_active || !m_queue.empty(); });
    if (!m_active) break;
    while (!m_queue.empty()) {
      if (!m_active) break;
      auto item = std::move(m_queue.front());
      m_queue.pop();
      auto func = m_func;
      auto mid = m_mid;
      lock.unlock();  // don't hold mutex during callback execution
      env->CallVoidMethod(func, mid, (jint)item.level,
                          ToJavaString(env, item.file), (jint)item.line,
                          ToJavaString(env, item.msg));
      if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
      }
      lock.lock();
    }
  }

  if (jvm) jvm->DetachCurrentThread();

  // use condition variable to signal thread shutdown
  {
    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    m_shutdown = true;
    m_shutdown_cv.notify_one();
  }
}

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setLogger
 * Signature: (Ledu/wpi/first/wpilibj/networktables/NetworkTablesJNI/LoggerFunction;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setLogger
  (JNIEnv *env, jclass, jobject func, jint minLevel)
{
  // cls is a temporary here; cannot be used within callback functor
	jclass cls = env->GetObjectClass(func);
  if (!cls) return;

  // method ids, on the other hand, are safe to retain
  jmethodID mid = env->GetMethodID(
      cls, "apply", "(ILjava/lang/String;ILjava/lang/String;)V");
  if (!mid) return;

  auto& thread = LoggerThreadJNI::GetInstance();
  thread.SetFunc(env, func, mid);
  thread.Start();

  nt::SetLogger(
      [](unsigned int level, const char *file, unsigned int line,
         const char *msg) {
        LoggerThreadJNI::GetInstance().Log(level, file, line, msg);
      },
      minLevel);
}

}  // extern "C"
