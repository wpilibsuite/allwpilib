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
#include "support/atomic_static.h"
#include "support/jni_util.h"
#include "support/SafeThread.h"
#include "llvm/ConvertUTF.h"
#include "llvm/SmallString.h"
#include "llvm/SmallVector.h"

using namespace wpi::java;

//
// Globals and load/unload
//

// Used for callback.
static JavaVM *jvm = nullptr;
static JClass booleanCls;
static JClass doubleCls;
static JClass connectionInfoCls;
static JClass entryInfoCls;
static JException keyNotDefinedEx;
static JException persistentEx;
static JException illegalArgEx;
static JException nullPointerEx;
// Thread-attached environment for listener callbacks.
static JNIEnv *listenerEnv = nullptr;

static void ListenerOnStart() {
  if (!jvm) return;
  JNIEnv *env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_2;
  args.name = const_cast<char*>("NTListener");
  args.group = nullptr;
  if (jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void **>(&env),
                                       &args) != JNI_OK)
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
  booleanCls = JClass(env, "java/lang/Boolean");
  if (!booleanCls) return JNI_ERR;

  doubleCls = JClass(env, "java/lang/Double");
  if (!doubleCls) return JNI_ERR;

  connectionInfoCls =
      JClass(env, "edu/wpi/first/wpilibj/networktables/ConnectionInfo");
  if (!connectionInfoCls) return JNI_ERR;

  entryInfoCls = JClass(env, "edu/wpi/first/wpilibj/networktables/EntryInfo");
  if (!entryInfoCls) return JNI_ERR;

  keyNotDefinedEx = JException(
      env, "edu/wpi/first/wpilibj/networktables/NetworkTableKeyNotDefined");
  if (!keyNotDefinedEx) return JNI_ERR;

  persistentEx = JException(
      env, "edu/wpi/first/wpilibj/networktables/PersistentException");
  if (!persistentEx) return JNI_ERR;

  illegalArgEx = JException(env, "java/lang/IllegalArgumentException");
  if (!illegalArgEx) return JNI_ERR;

  nullPointerEx = JException(env, "java/lang/NullPointerException");
  if (!nullPointerEx) return JNI_ERR;

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
  booleanCls.free(env);
  doubleCls.free(env);
  connectionInfoCls.free(env);
  entryInfoCls.free(env);
  keyNotDefinedEx.free(env);
  persistentEx.free(env);
  illegalArgEx.free(env);
  nullPointerEx.free(env);
  jvm = nullptr;
}

}  // extern "C"

//
// Helper class to create and clean up a global reference
//
template <typename T>
class JGlobal {
 public:
  JGlobal(JNIEnv *env, T obj)
      : m_obj(static_cast<T>(env->NewGlobalRef(obj))) {}
  ~JGlobal() {
    if (!jvm || nt::NotifierDestroyed()) return;
    JNIEnv *env;
    bool attached = false;
    // don't attach and de-attach if already attached to a thread.
    if (jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) ==
        JNI_EDETACHED) {
      if (jvm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr) !=
          JNI_OK)
        return;
      attached = true;
    }
    if (!env || !env->functions) return;
    env->DeleteGlobalRef(m_obj);
    if (attached) jvm->DetachCurrentThread();
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
class JWeakGlobal {
 public:
  JWeakGlobal(JNIEnv *env, T obj)
      : m_obj(static_cast<T>(env->NewWeakGlobalRef(obj))) {}
  ~JWeakGlobal() {
    if (!jvm || nt::NotifierDestroyed()) return;
    JNIEnv *env;
    bool attached = false;
    // don't attach and de-attach if already attached to a thread.
    if (jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) ==
        JNI_EDETACHED) {
      if (jvm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr) !=
          JNI_OK)
        return;
      attached = true;
    }
    if (!env || !env->functions) return;
    env->DeleteWeakGlobalRef(m_obj);
    if (attached) jvm->DetachCurrentThread();
  }
  JLocal<T> obj(JNIEnv *env) {
    return JLocal<T>{env, env->NewLocalRef(m_obj)};
  }

 private:
  T m_obj;
};

//
// Conversions from Java objects to C++
//

inline std::shared_ptr<nt::Value> FromJavaRaw(JNIEnv *env, jbyteArray jarr) {
  CriticalJByteArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  return nt::Value::MakeRaw(ref);
}

inline std::shared_ptr<nt::Value> FromJavaRawBB(JNIEnv *env, jobject jbb,
                                                int len) {
  JByteArrayRef ref{env, jbb, len};
  if (!ref) return nullptr;
  return nt::Value::MakeRaw(ref.str());
}

inline std::shared_ptr<nt::Value> FromJavaRpc(JNIEnv *env, jbyteArray jarr) {
  CriticalJByteArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  return nt::Value::MakeRpc(ref.str());
}

std::shared_ptr<nt::Value> FromJavaBooleanArray(JNIEnv *env,
                                                jbooleanArray jarr) {
  CriticalJBooleanArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  llvm::ArrayRef<jboolean> elements{ref};
  size_t len = elements.size();
  std::vector<int> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) arr.push_back(elements[i]);
  return nt::Value::MakeBooleanArray(arr);
}

std::shared_ptr<nt::Value> FromJavaDoubleArray(JNIEnv *env, jdoubleArray jarr) {
  CriticalJDoubleArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  return nt::Value::MakeDoubleArray(ref);
}

std::shared_ptr<nt::Value> FromJavaStringArray(JNIEnv *env, jobjectArray jarr) {
  size_t len = env->GetArrayLength(jarr);
  std::vector<std::string> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(jarr, i))};
    if (!elem) return nullptr;
    arr.push_back(JStringRef{env, elem}.str());
  }
  return nt::Value::MakeStringArray(std::move(arr));
}

//
// Conversions from C++ to Java objects
//

static jobject MakeJObject(JNIEnv *env, const nt::Value& value) {
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
      return MakeJString(env, value.GetString());
    case NT_RAW:
      return MakeJByteArray(env, value.GetRaw());
    case NT_BOOLEAN_ARRAY:
      return MakeJBooleanArray(env, value.GetBooleanArray());
    case NT_DOUBLE_ARRAY:
      return MakeJDoubleArray(env, value.GetDoubleArray());
    case NT_STRING_ARRAY:
      return MakeJStringArray(env, value.GetStringArray());
    case NT_RPC:
      return MakeJByteArray(env, value.GetRpc());
    default:
      return nullptr;
  }
}

static jobject MakeJObject(JNIEnv *env, const nt::ConnectionInfo &info) {
  static jmethodID constructor =
      env->GetMethodID(connectionInfoCls, "<init>",
                       "(Ljava/lang/String;Ljava/lang/String;IJI)V");
  JLocal<jstring> remote_id{env, MakeJString(env, info.remote_id)};
  JLocal<jstring> remote_ip{env, MakeJString(env, info.remote_ip)};
  return env->NewObject(connectionInfoCls, constructor, remote_id.obj(),
                        remote_ip.obj(), (jint)info.remote_port,
                        (jlong)info.last_update, (jint)info.protocol_version);
}

static jobject MakeJObject(JNIEnv *env, const nt::EntryInfo &info) {
  static jmethodID constructor =
      env->GetMethodID(entryInfoCls, "<init>", "(Ljava/lang/String;IIJ)V");
  JLocal<jstring> name{env, MakeJString(env, info.name)};
  return env->NewObject(entryInfoCls, constructor, name.obj(), (jint)info.type,
                        (jint)info.flags, (jlong)info.last_change);
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  return nt::SetEntryValue(JStringRef{env, key},
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  return nt::SetEntryValue(JStringRef{env, key},
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  return nt::SetEntryValue(JStringRef{env, key},
                           nt::Value::MakeString(JStringRef{env, value}));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putRaw
 * Signature: (Ljava/lang/String;[B)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putRaw__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaRaw(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putRaw
 * Signature: (Ljava/lang/String;Ljava/nio/ByteBuffer;I)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putRaw__Ljava_lang_String_2Ljava_nio_ByteBuffer_2I
  (JNIEnv *env, jclass, jstring key, jobject value, jint len)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaRawBB(env, value, len);
  if (!v) return false;
  return nt::SetEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putBooleanArray
 * Signature: (Ljava/lang/String;[Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putBooleanArray
  (JNIEnv *env, jclass, jstring key, jbooleanArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaBooleanArray(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putDoubleArray
 * Signature: (Ljava/lang/String;[D)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putDoubleArray
  (JNIEnv *env, jclass, jstring key, jdoubleArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaDoubleArray(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    putStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_putStringArray
  (JNIEnv *env, jclass, jstring key, jobjectArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaStringArray(env, value);
  if (!v) return false;
  return nt::SetEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutBoolean
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutBoolean
  (JNIEnv *env, jclass, jstring key, jboolean value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  nt::SetEntryTypeValue(JStringRef{env, key},
                        nt::Value::MakeBoolean(value != JNI_FALSE));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutDouble
 * Signature: (Ljava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutDouble
  (JNIEnv *env, jclass, jstring key, jdouble value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  nt::SetEntryTypeValue(JStringRef{env, key}, nt::Value::MakeDouble(value));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutString
  (JNIEnv *env, jclass, jstring key, jstring value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  nt::SetEntryTypeValue(JStringRef{env, key},
                        nt::Value::MakeString(JStringRef{env, value}));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutRaw
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutRaw__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  auto v = FromJavaRaw(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutRaw
 * Signature: (Ljava/lang/String;Ljava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutRaw__Ljava_lang_String_2Ljava_nio_ByteBuffer_2I
  (JNIEnv *env, jclass, jstring key, jobject value, jint len)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  auto v = FromJavaRawBB(env, value, len);
  if (!v) return;
  nt::SetEntryTypeValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutBooleanArray
 * Signature: (Ljava/lang/String;[Z)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutBooleanArray
  (JNIEnv *env, jclass, jstring key, jbooleanArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  auto v = FromJavaBooleanArray(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutDoubleArray
 * Signature: (Ljava/lang/String;[D)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutDoubleArray
  (JNIEnv *env, jclass, jstring key, jdoubleArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  auto v = FromJavaDoubleArray(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    forcePutStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_forcePutStringArray
  (JNIEnv *env, jclass, jstring key, jobjectArray value)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return;
  }
  auto v = FromJavaStringArray(env, value);
  if (!v) return;
  nt::SetEntryTypeValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getValue
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getValue__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJObject(env, *val);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBoolean
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBoolean__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsBoolean()) {
    keyNotDefinedEx.Throw(env, key);
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsDouble()) {
    keyNotDefinedEx.Throw(env, key);
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsString()) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJString(env, val->GetString());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRaw
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRaw__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsRaw()) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJByteArray(env, val->GetRaw());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBooleanArray
 * Signature: (Ljava/lang/String;)[Z
 */
JNIEXPORT jbooleanArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBooleanArray__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsBooleanArray()) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJBooleanArray(env, val->GetBooleanArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getDoubleArray
 * Signature: (Ljava/lang/String;)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getDoubleArray__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsDoubleArray()) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJDoubleArray(env, val->GetDoubleArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getStringArray
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getStringArray__Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsStringArray()) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJStringArray(env, val->GetStringArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getValue
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getValue__Ljava_lang_String_2Ljava_lang_Object_2
  (JNIEnv *env, jclass, jstring key, jobject defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val) return defaultValue;
  return MakeJObject(env, *val);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBoolean
 * Signature: (Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBoolean__Ljava_lang_String_2Z
  (JNIEnv *env, jclass, jstring key, jboolean defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsString()) return defaultValue;
  return MakeJString(env, val->GetString());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRaw
 * Signature: (Ljava/lang/String;[B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRaw__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsRaw()) return defaultValue;
  return MakeJByteArray(env, val->GetRaw());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getBooleanArray
 * Signature: (Ljava/lang/String;[Z)[Z
 */
JNIEXPORT jbooleanArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getBooleanArray__Ljava_lang_String_2_3Z
  (JNIEnv *env, jclass, jstring key, jbooleanArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsBooleanArray()) return defaultValue;
  return MakeJBooleanArray(env, val->GetBooleanArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getDoubleArray
 * Signature: (Ljava/lang/String;[D)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getDoubleArray__Ljava_lang_String_2_3D
  (JNIEnv *env, jclass, jstring key, jdoubleArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsDoubleArray()) return defaultValue;
  return MakeJDoubleArray(env, val->GetDoubleArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getStringArray__Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *env, jclass, jstring key, jobjectArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsStringArray()) return defaultValue;
  return MakeJStringArray(env, val->GetStringArray());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultBoolean
 * Signature: (Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultBoolean
  (JNIEnv *env, jclass, jstring key, jboolean defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  return nt::SetDefaultEntryValue(JStringRef{env, key}, 
                                  nt::Value::MakeBoolean(defaultValue != JNI_FALSE));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultDouble
 * Signature: (Ljava/lang/String;D)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultDouble
  (JNIEnv *env, jclass, jstring key, jdouble defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  return nt::SetDefaultEntryValue(JStringRef{env, key},
                                  nt::Value::MakeDouble(defaultValue));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultString
  (JNIEnv *env, jclass, jstring key, jstring defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  return nt::SetDefaultEntryValue(
      JStringRef{env, key},
      nt::Value::MakeString(JStringRef{env, defaultValue}));
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultRaw
 * Signature: (Ljava/lang/String;[B)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultRaw
  (JNIEnv *env, jclass, jstring key, jbyteArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaRaw(env, defaultValue);
  return nt::SetDefaultEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultBooleanArray
 * Signature: (Ljava/lang/String;[Z)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultBooleanArray
  (JNIEnv *env, jclass, jstring key, jbooleanArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaBooleanArray(env, defaultValue);
  return nt::SetDefaultEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultDoubleArray
 * Signature: (Ljava/lang/String;[D)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultDoubleArray
  (JNIEnv *env, jclass, jstring key, jdoubleArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaDoubleArray(env, defaultValue);
  return nt::SetDefaultEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setDefaultStringArray
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setDefaultStringArray
  (JNIEnv *env, jclass, jstring key, jobjectArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaStringArray(env, defaultValue);
  return nt::SetDefaultEntryValue(JStringRef{env, key}, v);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setEntryFlags
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setEntryFlags
  (JNIEnv *env, jclass, jstring key, jint flags)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  nt::SetEntryFlags(JStringRef{env, key}, flags);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getEntryFlags
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getEntryFlags
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  return nt::GetEntryFlags(JStringRef{env, key});
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    deleteEntry
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_deleteEntry
  (JNIEnv *env, jclass, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return;
  }
  nt::DeleteEntry(JStringRef{env, key});
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
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  auto arr = nt::GetEntryInfo(JStringRef{env, prefix}, types);
  jobjectArray jarr = env->NewObjectArray(arr.size(), entryInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> jelem{env, MakeJObject(env, arr[i])};
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
  if (!prefix) {
    nullPointerEx.Throw(envouter, "prefix cannot be null");
    return 0;
  }
  if (!listener) {
    nullPointerEx.Throw(envouter, "listener cannot be null");
    return 0;
  }

  // the shared pointer to the weak global will keep it around until the
  // entry listener is destroyed
  auto listener_global =
      std::make_shared<JGlobal<jobject>>(envouter, listener);

  // cls is a temporary here; cannot be used within callback functor
	jclass cls = envouter->GetObjectClass(listener);
  if (!cls) return 0;

  // method ids, on the other hand, are safe to retain
  jmethodID mid = envouter->GetMethodID(
      cls, "apply", "(ILjava/lang/String;Ljava/lang/Object;I)V");
  if (!mid) return 0;

  return nt::AddEntryListener(
      JStringRef{envouter, prefix},
      [=](unsigned int uid, nt::StringRef name,
          std::shared_ptr<nt::Value> value, unsigned int flags_) {
        JNIEnv *env = listenerEnv;
        if (!env || !env->functions) return;

        // get the handler
        auto handler = listener_global->obj();

        // convert the value into the appropriate Java type
        JLocal<jobject> jobj{env, MakeJObject(env, *value)};
        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
          return;
        }
        if (!jobj) return;

        JLocal<jstring> jname{env, MakeJString(env, name)};
        env->CallVoidMethod(handler, mid, (jint)uid, jname.obj(), jobj.obj(),
                            (jint)(flags_));
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
  if (!listener) {
    nullPointerEx.Throw(envouter, "listener cannot be null");
    return 0;
  }

  // the shared pointer to the weak global will keep it around until the
  // entry listener is destroyed
  auto listener_global =
      std::make_shared<JGlobal<jobject>>(envouter, listener);

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
        JLocal<jobject> jobj{env, MakeJObject(env, conn)};
        if (env->ExceptionCheck()) {
          env->ExceptionDescribe();
          env->ExceptionClear();
          return;
        }
        if (!jobj) return;

        env->CallVoidMethod(handler, mid, (jint)uid,
                            (jboolean)(connected ? 1 : 0), jobj.obj());
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
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsRpc()) {
    keyNotDefinedEx.Throw(env, key);
    return nullptr;
  }
  return MakeJByteArray(env, val->GetRpc());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    getRpc
 * Signature: (Ljava/lang/String;[B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_getRpc__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray defaultValue)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return nullptr;
  }
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return nullptr;
  }
  auto val = nt::GetEntryValue(JStringRef{env, key});
  if (!val || !val->IsRpc()) return defaultValue;
  return MakeJByteArray(env, val->GetRpc());
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    callRpc
 * Signature: (Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_callRpc__Ljava_lang_String_2_3B
  (JNIEnv *env, jclass, jstring key, jbyteArray params)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  if (!params) {
    nullPointerEx.Throw(env, "params cannot be null");
    return 0;
  }
  return nt::CallRpc(JStringRef{env, key}, JByteArrayRef{env, params});
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    callRpc
 * Signature: (Ljava/lang/String;Ljava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_callRpc__Ljava_lang_String_2Ljava_nio_ByteBuffer_2I
  (JNIEnv *env, jclass, jstring key, jobject params, jint params_len)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return 0;
  }
  if (!params) {
    nullPointerEx.Throw(env, "params cannot be null");
    return 0;
  }
  return nt::CallRpc(JStringRef{env, key},
                     JByteArrayRef{env, params, params_len});
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setNetworkIdentity
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setNetworkIdentity
  (JNIEnv *env, jclass, jstring name)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return;
  }
  nt::SetNetworkIdentity(JStringRef{env, name});
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
  if (!persistFilename) {
    nullPointerEx.Throw(env, "persistFilename cannot be null");
    return;
  }
  if (!listenAddress) {
    nullPointerEx.Throw(env, "listenAddress cannot be null");
    return;
  }
  nt::StartServer(JStringRef{env, persistFilename},
                  JStringRef{env, listenAddress}.c_str(), port);
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
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_startClient__
  (JNIEnv *env, jclass)
{
  nt::StartClient();
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_startClient__Ljava_lang_String_2I
  (JNIEnv *env, jclass, jstring serverName, jint port)
{
  if (!serverName) {
    nullPointerEx.Throw(env, "serverName cannot be null");
    return;
  }
  nt::StartClient(JStringRef{env, serverName}.c_str(), port);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: ([Ljava/lang/String;[I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_startClient___3Ljava_lang_String_2_3I
  (JNIEnv *env, jclass, jobjectArray serverNames, jintArray ports)
{
  if (!serverNames) {
    nullPointerEx.Throw(env, "serverNames cannot be null");
    return;
  }
  if (!ports) {
    nullPointerEx.Throw(env, "ports cannot be null");
    return;
  }
  int len = env->GetArrayLength(serverNames);
  if (len != env->GetArrayLength(ports)) {
    illegalArgEx.Throw(env,
                       "serverNames and ports arrays must be the same size");
    return;
  }
  jint* portInts = env->GetIntArrayElements(ports, nullptr);
  if (!portInts) return;

  std::vector<std::string> names;
  std::vector<std::pair<nt::StringRef, unsigned int>> servers;
  names.reserve(len);
  servers.reserve(len);
  for (int i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(serverNames, i))};
    if (!elem) {
      nullPointerEx.Throw(env, "null string in serverNames");
      return;
    }
    names.emplace_back(JStringRef{env, elem}.str());
    servers.emplace_back(std::make_pair(nt::StringRef(names.back()),
                                        portInts[i]));
  }
  env->ReleaseIntArrayElements(ports, portInts, JNI_ABORT);
  nt::StartClient(servers);
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
 * Method:    setServer
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setServer__Ljava_lang_String_2I
  (JNIEnv *env, jclass, jstring serverName, jint port)
{
  if (!serverName) {
    nullPointerEx.Throw(env, "serverName cannot be null");
    return;
  }
  nt::SetServer(JStringRef{env, serverName}.c_str(), port);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setServer
 * Signature: ([Ljava/lang/String;[I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setServer___3Ljava_lang_String_2_3I
  (JNIEnv *env, jclass, jobjectArray serverNames, jintArray ports)
{
  if (!serverNames) {
    nullPointerEx.Throw(env, "serverNames cannot be null");
    return;
  }
  if (!ports) {
    nullPointerEx.Throw(env, "ports cannot be null");
    return;
  }
  int len = env->GetArrayLength(serverNames);
  if (len != env->GetArrayLength(ports)) {
    illegalArgEx.Throw(env,
                       "serverNames and ports arrays must be the same size");
    return;
  }
  jint* portInts = env->GetIntArrayElements(ports, nullptr);
  if (!portInts) return;

  std::vector<std::string> names;
  std::vector<std::pair<nt::StringRef, unsigned int>> servers;
  names.reserve(len);
  servers.reserve(len);
  for (int i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(serverNames, i))};
    if (!elem) {
      nullPointerEx.Throw(env, "null string in serverNames");
      return;
    }
    names.emplace_back(JStringRef{env, elem}.str());
    servers.emplace_back(std::make_pair(nt::StringRef(names.back()),
                                        portInts[i]));
  }
  env->ReleaseIntArrayElements(ports, portInts, JNI_ABORT);
  nt::SetServer(servers);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    startDSClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_startDSClient
  (JNIEnv *env, jclass, jint port)
{
  nt::StartDSClient(port);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    stopDSClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_stopDSClient
  (JNIEnv *env, jclass)
{
  nt::StopDSClient();
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
    JLocal<jobject> jelem{env, MakeJObject(env, arr[i])};
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
  if (!filename) {
    nullPointerEx.Throw(env, "filename cannot be null");
    return;
  }
  const char *err = nt::SavePersistent(JStringRef{env, filename});
  if (err) persistentEx.Throw(env, err);
}

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    loadPersistent
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_loadPersistent
  (JNIEnv *env, jclass, jstring filename)
{
  if (!filename) {
    nullPointerEx.Throw(env, "filename cannot be null");
    return nullptr;
  }
  std::vector<std::string> warns;
  const char *err = nt::LoadPersistent(JStringRef{env, filename},
                                       [&](size_t line, const char *msg) {
                                         std::ostringstream oss;
                                         oss << line << ": " << msg;
                                         warns.push_back(oss.str());
                                       });
  if (err) {
    persistentEx.Throw(env, err);
    return nullptr;
  }
  return MakeJStringArray(env, warns);
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

namespace {

struct LogMessage {
 public:
  LogMessage(unsigned int level, const char *file, unsigned int line,
             const char *msg)
      : m_level(level), m_file(file), m_line(line), m_msg(msg) {}

  void CallJava(JNIEnv* env, jobject func, jmethodID mid) {
    JLocal<jstring> file{env, MakeJString(env, m_file)};
    JLocal<jstring> msg{env, MakeJString(env, m_msg)};
    env->CallVoidMethod(func, mid, (jint)m_level, file.obj(),
                        (jint)m_line, msg.obj());
  }

  static const char* GetName() { return "NTLogger"; }
  static JavaVM* GetJVM() { return jvm; }

 private:
  unsigned int m_level;
  const char* m_file;
  unsigned int m_line;
  std::string m_msg;
};

typedef JSingletonCallbackManager<LogMessage> LoggerJNI;

}  // anonymous namespace

ATOMIC_STATIC_INIT(LoggerJNI)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_networktables_NetworkTablesJNI
 * Method:    setLogger
 * Signature: (Ledu/wpi/first/wpilibj/networktables/NetworkTablesJNI/LoggerFunction;I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_networktables_NetworkTablesJNI_setLogger
  (JNIEnv *env, jclass, jobject func, jint minLevel)
{
  if (!func) {
    nullPointerEx.Throw(env, "func cannot be null");
    return;
  }

  // cls is a temporary here; cannot be used within callback functor
	jclass cls = env->GetObjectClass(func);
  if (!cls) return;

  // method ids, on the other hand, are safe to retain
  jmethodID mid = env->GetMethodID(
      cls, "apply", "(ILjava/lang/String;ILjava/lang/String;)V");
  if (!mid) return;

  auto& logger = LoggerJNI::GetInstance();
  logger.Start();
  logger.SetFunc(env, func, mid);

  nt::SetLogger(
      [](unsigned int level, const char *file, unsigned int line,
         const char *msg) {
        LoggerJNI::GetInstance().Send(level, file, line, msg);
      },
      minLevel);
}

}  // extern "C"
