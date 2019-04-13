/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include <wpi/ConvertUTF.h>
#include <wpi/SmallString.h>
#include <wpi/jni_util.h>
#include <wpi/raw_ostream.h>

#include "edu_wpi_first_networktables_NetworkTablesJNI.h"
#include "ntcore.h"

using namespace wpi::java;

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

//
// Globals and load/unload
//

// Used for callback.
static JavaVM* jvm = nullptr;
static JClass booleanCls;
static JClass connectionInfoCls;
static JClass connectionNotificationCls;
static JClass doubleCls;
static JClass entryInfoCls;
static JClass entryNotificationCls;
static JClass logMessageCls;
static JClass rpcAnswerCls;
static JClass valueCls;
static JException illegalArgEx;
static JException interruptedEx;
static JException nullPointerEx;
static JException persistentEx;

static const JClassInit classes[] = {
    {"java/lang/Boolean", &booleanCls},
    {"edu/wpi/first/networktables/ConnectionInfo", &connectionInfoCls},
    {"edu/wpi/first/networktables/ConnectionNotification",
     &connectionNotificationCls},
    {"java/lang/Double", &doubleCls},
    {"edu/wpi/first/networktables/EntryInfo", &entryInfoCls},
    {"edu/wpi/first/networktables/EntryNotification", &entryNotificationCls},
    {"edu/wpi/first/networktables/LogMessage", &logMessageCls},
    {"edu/wpi/first/networktables/RpcAnswer", &rpcAnswerCls},
    {"edu/wpi/first/networktables/NetworkTableValue", &valueCls}};

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgEx},
    {"java/lang/InterruptedException", &interruptedEx},
    {"java/lang/NullPointerException", &nullPointerEx},
    {"edu/wpi/first/networktables/PersistentException", &persistentEx}};

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  // Cache references to classes
  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) return JNI_ERR;
  }

  for (auto& c : exceptions) {
    *c.cls = JException(env, c.name);
    if (!*c.cls) return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;
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
// Conversions from Java objects to C++
//

inline std::shared_ptr<nt::Value> FromJavaRaw(JNIEnv* env, jbyteArray jarr,
                                              jlong time) {
  CriticalJByteArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  return nt::Value::MakeRaw(ref, time);
}

inline std::shared_ptr<nt::Value> FromJavaRawBB(JNIEnv* env, jobject jbb,
                                                int len, jlong time) {
  JByteArrayRef ref{env, jbb, len};
  if (!ref) return nullptr;
  return nt::Value::MakeRaw(ref.str(), time);
}

inline std::shared_ptr<nt::Value> FromJavaRpc(JNIEnv* env, jbyteArray jarr,
                                              jlong time) {
  CriticalJByteArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  return nt::Value::MakeRpc(ref.str(), time);
}

std::shared_ptr<nt::Value> FromJavaBooleanArray(JNIEnv* env, jbooleanArray jarr,
                                                jlong time) {
  CriticalJBooleanArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  wpi::ArrayRef<jboolean> elements{ref};
  size_t len = elements.size();
  std::vector<int> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) arr.push_back(elements[i]);
  return nt::Value::MakeBooleanArray(arr, time);
}

std::shared_ptr<nt::Value> FromJavaDoubleArray(JNIEnv* env, jdoubleArray jarr,
                                               jlong time) {
  CriticalJDoubleArrayRef ref{env, jarr};
  if (!ref) return nullptr;
  return nt::Value::MakeDoubleArray(ref, time);
}

std::shared_ptr<nt::Value> FromJavaStringArray(JNIEnv* env, jobjectArray jarr,
                                               jlong time) {
  size_t len = env->GetArrayLength(jarr);
  std::vector<std::string> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(jarr, i))};
    if (!elem) return nullptr;
    arr.push_back(JStringRef{env, elem}.str());
  }
  return nt::Value::MakeStringArray(std::move(arr), time);
}

//
// Conversions from C++ to Java objects
//

static jobject MakeJObject(JNIEnv* env, const nt::Value& value) {
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

static jobject MakeJValue(JNIEnv* env, const nt::Value* value) {
  static jmethodID constructor =
      env->GetMethodID(valueCls, "<init>", "(ILjava/lang/Object;J)V");
  if (!value)
    return env->NewObject(valueCls, constructor, (jint)NT_UNASSIGNED, nullptr,
                          (jlong)0);
  return env->NewObject(valueCls, constructor, (jint)value->type(),
                        MakeJObject(env, *value), (jlong)value->time());
}

static jobject MakeJObject(JNIEnv* env, const nt::ConnectionInfo& info) {
  static jmethodID constructor =
      env->GetMethodID(connectionInfoCls, "<init>",
                       "(Ljava/lang/String;Ljava/lang/String;IJI)V");
  JLocal<jstring> remote_id{env, MakeJString(env, info.remote_id)};
  JLocal<jstring> remote_ip{env, MakeJString(env, info.remote_ip)};
  return env->NewObject(connectionInfoCls, constructor, remote_id.obj(),
                        remote_ip.obj(), (jint)info.remote_port,
                        (jlong)info.last_update, (jint)info.protocol_version);
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const nt::ConnectionNotification& notification) {
  static jmethodID constructor = env->GetMethodID(
      connectionNotificationCls, "<init>",
      "(Ledu/wpi/first/networktables/NetworkTableInstance;IZLedu/wpi/first/"
      "networktables/ConnectionInfo;)V");
  JLocal<jobject> conn{env, MakeJObject(env, notification.conn)};
  return env->NewObject(connectionNotificationCls, constructor, inst,
                        (jint)notification.listener,
                        (jboolean)notification.connected, conn.obj());
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const nt::EntryInfo& info) {
  static jmethodID constructor =
      env->GetMethodID(entryInfoCls, "<init>",
                       "(Ledu/wpi/first/networktables/"
                       "NetworkTableInstance;ILjava/lang/String;IIJ)V");
  JLocal<jstring> name{env, MakeJString(env, info.name)};
  return env->NewObject(entryInfoCls, constructor, inst, (jint)info.entry,
                        name.obj(), (jint)info.type, (jint)info.flags,
                        (jlong)info.last_change);
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const nt::EntryNotification& notification) {
  static jmethodID constructor = env->GetMethodID(
      entryNotificationCls, "<init>",
      "(Ledu/wpi/first/networktables/NetworkTableInstance;IILjava/lang/"
      "String;Ledu/wpi/first/networktables/NetworkTableValue;I)V");
  JLocal<jstring> name{env, MakeJString(env, notification.name)};
  JLocal<jobject> value{env, MakeJValue(env, notification.value.get())};
  return env->NewObject(entryNotificationCls, constructor, inst,
                        (jint)notification.listener, (jint)notification.entry,
                        name.obj(), value.obj(), (jint)notification.flags);
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const nt::LogMessage& msg) {
  static jmethodID constructor = env->GetMethodID(
      logMessageCls, "<init>",
      "(Ledu/wpi/first/networktables/NetworkTableInstance;IILjava/lang/"
      "String;ILjava/lang/String;)V");
  JLocal<jstring> filename{env, MakeJString(env, msg.filename)};
  JLocal<jstring> message{env, MakeJString(env, msg.message)};
  return env->NewObject(logMessageCls, constructor, inst, (jint)msg.logger,
                        (jint)msg.level, filename.obj(), (jint)msg.line,
                        message.obj());
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const nt::RpcAnswer& answer) {
  static jmethodID constructor =
      env->GetMethodID(rpcAnswerCls, "<init>",
                       "(Ledu/wpi/first/networktables/"
                       "NetworkTableInstance;IILjava/lang/String;[B"
                       "Ledu/wpi/first/networktables/ConnectionInfo;)V");
  JLocal<jstring> name{env, MakeJString(env, answer.name)};
  JLocal<jbyteArray> params{env, MakeJByteArray(env, answer.params)};
  JLocal<jobject> conn{env, MakeJObject(env, answer.conn)};
  return env->NewObject(rpcAnswerCls, constructor, inst, (jint)answer.entry,
                        (jint)answer.call, name.obj(), params.obj(),
                        conn.obj());
}

static jobjectArray MakeJObject(JNIEnv* env, jobject inst,
                                wpi::ArrayRef<nt::ConnectionNotification> arr) {
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), connectionNotificationCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, inst, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobjectArray MakeJObject(JNIEnv* env, jobject inst,
                                wpi::ArrayRef<nt::EntryNotification> arr) {
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), entryNotificationCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, inst, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobjectArray MakeJObject(JNIEnv* env, jobject inst,
                                wpi::ArrayRef<nt::LogMessage> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), logMessageCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, inst, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobjectArray MakeJObject(JNIEnv* env, jobject inst,
                                wpi::ArrayRef<nt::RpcAnswer> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), rpcAnswerCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, inst, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

extern "C" {

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getDefaultInstance
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getDefaultInstance
  (JNIEnv*, jclass)
{
  return nt::GetDefaultInstance();
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    createInstance
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_createInstance
  (JNIEnv*, jclass)
{
  return nt::CreateInstance();
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    destroyInstance
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_destroyInstance
  (JNIEnv*, jclass, jint inst)
{
  nt::DestroyInstance(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getInstanceFromHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getInstanceFromHandle
  (JNIEnv*, jclass, jint handle)
{
  return nt::GetInstanceFromHandle(handle);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntry
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntry
  (JNIEnv* env, jclass, jint inst, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  return nt::GetEntry(inst, JStringRef{env, key}.str());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntries
 * Signature: (ILjava/lang/String;I)[I
 */
JNIEXPORT jintArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntries
  (JNIEnv* env, jclass, jint inst, jstring prefix, jint types)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  return MakeJIntArray(
      env, nt::GetEntries(inst, JStringRef{env, prefix}.str(), types));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntryName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntryName
  (JNIEnv* env, jclass, jint entry)
{
  return MakeJString(env, nt::GetEntryName(entry));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntryLastChange
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntryLastChange
  (JNIEnv*, jclass, jint entry)
{
  return nt::GetEntryLastChange(entry);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getType
  (JNIEnv*, jclass, jint entry)
{
  return nt::GetEntryType(entry);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setBoolean
 * Signature: (IJZZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setBoolean
  (JNIEnv*, jclass, jint entry, jlong time, jboolean value, jboolean force)
{
  if (force) {
    nt::SetEntryTypeValue(entry,
                          nt::Value::MakeBoolean(value != JNI_FALSE, time));
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry,
                           nt::Value::MakeBoolean(value != JNI_FALSE, time));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDouble
 * Signature: (IJDZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDouble
  (JNIEnv*, jclass, jint entry, jlong time, jdouble value, jboolean force)
{
  if (force) {
    nt::SetEntryTypeValue(entry, nt::Value::MakeDouble(value, time));
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry, nt::Value::MakeDouble(value, time));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setString
 * Signature: (IJLjava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setString
  (JNIEnv* env, jclass, jint entry, jlong time, jstring value, jboolean force)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  if (force) {
    nt::SetEntryTypeValue(
        entry, nt::Value::MakeString(JStringRef{env, value}.str(), time));
    return JNI_TRUE;
  }
  return nt::SetEntryValue(
      entry, nt::Value::MakeString(JStringRef{env, value}.str(), time));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setRaw
 * Signature: (IJ[BZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setRaw__IJ_3BZ
  (JNIEnv* env, jclass, jint entry, jlong time, jbyteArray value,
   jboolean force)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaRaw(env, value, time);
  if (!v) return false;
  if (force) {
    nt::SetEntryTypeValue(entry, v);
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setRaw
 * Signature: (IJLjava/lang/Object;IZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setRaw__IJLjava_nio_ByteBuffer_2IZ
  (JNIEnv* env, jclass, jint entry, jlong time, jobject value, jint len,
   jboolean force)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaRawBB(env, value, len, time);
  if (!v) return false;
  if (force) {
    nt::SetEntryTypeValue(entry, v);
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setBooleanArray
 * Signature: (IJ[ZZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setBooleanArray
  (JNIEnv* env, jclass, jint entry, jlong time, jbooleanArray value,
   jboolean force)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaBooleanArray(env, value, time);
  if (!v) return false;
  if (force) {
    nt::SetEntryTypeValue(entry, v);
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDoubleArray
 * Signature: (IJ[DZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDoubleArray
  (JNIEnv* env, jclass, jint entry, jlong time, jdoubleArray value,
   jboolean force)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaDoubleArray(env, value, time);
  if (!v) return false;
  if (force) {
    nt::SetEntryTypeValue(entry, v);
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setStringArray
 * Signature: (IJ[Ljava/lang/Object;Z)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setStringArray
  (JNIEnv* env, jclass, jint entry, jlong time, jobjectArray value,
   jboolean force)
{
  if (!value) {
    nullPointerEx.Throw(env, "value cannot be null");
    return false;
  }
  auto v = FromJavaStringArray(env, value, time);
  if (!v) return false;
  if (force) {
    nt::SetEntryTypeValue(entry, v);
    return JNI_TRUE;
  }
  return nt::SetEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getValue
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getValue
  (JNIEnv* env, jclass, jint entry)
{
  auto val = nt::GetEntryValue(entry);
  return MakeJValue(env, val.get());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getBoolean
 * Signature: (IZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getBoolean
  (JNIEnv*, jclass, jint entry, jboolean defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsBoolean()) return defaultValue;
  return val->GetBoolean();
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getDouble
 * Signature: (ID)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getDouble
  (JNIEnv*, jclass, jint entry, jdouble defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsDouble()) return defaultValue;
  return val->GetDouble();
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getString
 * Signature: (ILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getString
  (JNIEnv* env, jclass, jint entry, jstring defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsString()) return defaultValue;
  return MakeJString(env, val->GetString());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getRaw
 * Signature: (I[B)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getRaw
  (JNIEnv* env, jclass, jint entry, jbyteArray defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsRaw()) return defaultValue;
  return MakeJByteArray(env, val->GetRaw());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getBooleanArray
 * Signature: (I[Z)[Z
 */
JNIEXPORT jbooleanArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getBooleanArray
  (JNIEnv* env, jclass, jint entry, jbooleanArray defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsBooleanArray()) return defaultValue;
  return MakeJBooleanArray(env, val->GetBooleanArray());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getDoubleArray
 * Signature: (I[D)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getDoubleArray
  (JNIEnv* env, jclass, jint entry, jdoubleArray defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsDoubleArray()) return defaultValue;
  return MakeJDoubleArray(env, val->GetDoubleArray());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getStringArray
 * Signature: (I[Ljava/lang/Object;)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getStringArray
  (JNIEnv* env, jclass, jint entry, jobjectArray defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsStringArray()) return defaultValue;
  return MakeJStringArray(env, val->GetStringArray());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultBoolean
 * Signature: (IJZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultBoolean
  (JNIEnv*, jclass, jint entry, jlong time, jboolean defaultValue)
{
  return nt::SetDefaultEntryValue(
      entry, nt::Value::MakeBoolean(defaultValue != JNI_FALSE, time));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultDouble
 * Signature: (IJD)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultDouble
  (JNIEnv*, jclass, jint entry, jlong time, jdouble defaultValue)
{
  return nt::SetDefaultEntryValue(entry,
                                  nt::Value::MakeDouble(defaultValue, time));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultString
 * Signature: (IJLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultString
  (JNIEnv* env, jclass, jint entry, jlong time, jstring defaultValue)
{
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  return nt::SetDefaultEntryValue(
      entry, nt::Value::MakeString(JStringRef{env, defaultValue}.str(), time));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultRaw
 * Signature: (IJ[B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultRaw
  (JNIEnv* env, jclass, jint entry, jlong time, jbyteArray defaultValue)
{
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaRaw(env, defaultValue, time);
  return nt::SetDefaultEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultBooleanArray
 * Signature: (IJ[Z)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultBooleanArray
  (JNIEnv* env, jclass, jint entry, jlong time, jbooleanArray defaultValue)
{
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaBooleanArray(env, defaultValue, time);
  return nt::SetDefaultEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultDoubleArray
 * Signature: (IJ[D)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultDoubleArray
  (JNIEnv* env, jclass, jint entry, jlong time, jdoubleArray defaultValue)
{
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaDoubleArray(env, defaultValue, time);
  return nt::SetDefaultEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setDefaultStringArray
 * Signature: (IJ[Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setDefaultStringArray
  (JNIEnv* env, jclass, jint entry, jlong time, jobjectArray defaultValue)
{
  if (!defaultValue) {
    nullPointerEx.Throw(env, "defaultValue cannot be null");
    return false;
  }
  auto v = FromJavaStringArray(env, defaultValue, time);
  return nt::SetDefaultEntryValue(entry, v);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setEntryFlags
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setEntryFlags
  (JNIEnv*, jclass, jint entry, jint flags)
{
  nt::SetEntryFlags(entry, flags);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntryFlags
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntryFlags
  (JNIEnv*, jclass, jint entry)
{
  return nt::GetEntryFlags(entry);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    deleteEntry
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_deleteEntry
  (JNIEnv*, jclass, jint entry)
{
  nt::DeleteEntry(entry);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    deleteAllEntries
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_deleteAllEntries
  (JNIEnv*, jclass, jint inst)
{
  nt::DeleteAllEntries(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntryInfoHandle
 * Signature: (Ljava/lang/Object;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntryInfoHandle
  (JNIEnv* env, jclass, jobject inst, jint entry)
{
  return MakeJObject(env, inst, nt::GetEntryInfo(entry));
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getEntryInfo
 * Signature: (Ljava/lang/Object;ILjava/lang/String;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getEntryInfo
  (JNIEnv* env, jclass, jobject instObject, jint inst, jstring prefix,
   jint types)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  auto arr = nt::GetEntryInfo(inst, JStringRef{env, prefix}.str(), types);
  jobjectArray jarr = env->NewObjectArray(arr.size(), entryInfoCls, nullptr);
  if (!jarr) return nullptr;
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> jelem{env, MakeJObject(env, instObject, arr[i])};
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    createEntryListenerPoller
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_createEntryListenerPoller
  (JNIEnv*, jclass, jint inst)
{
  return nt::CreateEntryListenerPoller(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    destroyEntryListenerPoller
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_destroyEntryListenerPoller
  (JNIEnv*, jclass, jint poller)
{
  nt::DestroyEntryListenerPoller(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    addPolledEntryListener
 * Signature: (ILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_addPolledEntryListener__ILjava_lang_String_2I
  (JNIEnv* env, jclass, jint poller, jstring prefix, jint flags)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return 0;
  }
  return nt::AddPolledEntryListener(poller, JStringRef{env, prefix}.str(),
                                    flags);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    addPolledEntryListener
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_addPolledEntryListener__III
  (JNIEnv* env, jclass, jint poller, jint entry, jint flags)
{
  return nt::AddPolledEntryListener(poller, entry, flags);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollEntryListener
 * Signature: (Ljava/lang/Object;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollEntryListener
  (JNIEnv* env, jclass, jobject inst, jint poller)
{
  auto events = nt::PollEntryListener(poller);
  if (events.empty()) {
    interruptedEx.Throw(env, "PollEntryListener interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, events);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollEntryListenerTimeout
 * Signature: (Ljava/lang/Object;ID)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollEntryListenerTimeout
  (JNIEnv* env, jclass, jobject inst, jint poller, jdouble timeout)
{
  bool timed_out = false;
  auto events = nt::PollEntryListener(poller, timeout, &timed_out);
  if (events.empty() && !timed_out) {
    interruptedEx.Throw(env, "PollEntryListener interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, events);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    cancelPollEntryListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_cancelPollEntryListener
  (JNIEnv*, jclass, jint poller)
{
  nt::CancelPollEntryListener(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    removeEntryListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_removeEntryListener
  (JNIEnv*, jclass, jint entryListenerUid)
{
  nt::RemoveEntryListener(entryListenerUid);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    waitForEntryListenerQueue
 * Signature: (ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_waitForEntryListenerQueue
  (JNIEnv*, jclass, jint inst, jdouble timeout)
{
  return nt::WaitForEntryListenerQueue(inst, timeout);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    createConnectionListenerPoller
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_createConnectionListenerPoller
  (JNIEnv*, jclass, jint inst)
{
  return nt::CreateConnectionListenerPoller(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    destroyConnectionListenerPoller
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_destroyConnectionListenerPoller
  (JNIEnv*, jclass, jint poller)
{
  nt::DestroyConnectionListenerPoller(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    addPolledConnectionListener
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_addPolledConnectionListener
  (JNIEnv* env, jclass, jint poller, jboolean immediateNotify)
{
  return nt::AddPolledConnectionListener(poller, immediateNotify);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollConnectionListener
 * Signature: (Ljava/lang/Object;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollConnectionListener
  (JNIEnv* env, jclass, jobject inst, jint poller)
{
  auto events = nt::PollConnectionListener(poller);
  if (events.empty()) {
    interruptedEx.Throw(env, "PollConnectionListener interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, events);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollConnectionListenerTimeout
 * Signature: (Ljava/lang/Object;ID)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollConnectionListenerTimeout
  (JNIEnv* env, jclass, jobject inst, jint poller, jdouble timeout)
{
  bool timed_out = false;
  auto events = nt::PollConnectionListener(poller, timeout, &timed_out);
  if (events.empty() && !timed_out) {
    interruptedEx.Throw(env, "PollConnectionListener interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, events);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    cancelPollConnectionListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_cancelPollConnectionListener
  (JNIEnv*, jclass, jint poller)
{
  nt::CancelPollConnectionListener(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    removeConnectionListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_removeConnectionListener
  (JNIEnv*, jclass, jint connListenerUid)
{
  nt::RemoveConnectionListener(connListenerUid);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    waitForConnectionListenerQueue
 * Signature: (ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_waitForConnectionListenerQueue
  (JNIEnv*, jclass, jint inst, jdouble timeout)
{
  return nt::WaitForConnectionListenerQueue(inst, timeout);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    createRpcCallPoller
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_createRpcCallPoller
  (JNIEnv*, jclass, jint inst)
{
  return nt::CreateRpcCallPoller(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    destroyRpcCallPoller
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_destroyRpcCallPoller
  (JNIEnv*, jclass, jint poller)
{
  nt::DestroyRpcCallPoller(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    createPolledRpc
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_createPolledRpc
  (JNIEnv* env, jclass, jint entry, jbyteArray def, jint poller)
{
  if (!def) {
    nullPointerEx.Throw(env, "def cannot be null");
    return;
  }
  nt::CreatePolledRpc(entry, JByteArrayRef{env, def}, poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollRpc
 * Signature: (Ljava/lang/Object;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollRpc
  (JNIEnv* env, jclass, jobject inst, jint poller)
{
  auto infos = nt::PollRpc(poller);
  if (infos.empty()) {
    interruptedEx.Throw(env, "PollRpc interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, infos);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollRpcTimeout
 * Signature: (Ljava/lang/Object;ID)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollRpcTimeout
  (JNIEnv* env, jclass, jobject inst, jint poller, jdouble timeout)
{
  bool timed_out = false;
  auto infos = nt::PollRpc(poller, timeout, &timed_out);
  if (infos.empty() && !timed_out) {
    interruptedEx.Throw(env, "PollRpc interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, infos);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    cancelPollRpc
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_cancelPollRpc
  (JNIEnv*, jclass, jint poller)
{
  nt::CancelPollRpc(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    waitForRpcCallQueue
 * Signature: (ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_waitForRpcCallQueue
  (JNIEnv*, jclass, jint inst, jdouble timeout)
{
  return nt::WaitForRpcCallQueue(inst, timeout);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    postRpcResponse
 * Signature: (II[B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_postRpcResponse
  (JNIEnv* env, jclass, jint entry, jint call, jbyteArray result)
{
  if (!result) {
    nullPointerEx.Throw(env, "result cannot be null");
    return false;
  }
  return nt::PostRpcResponse(entry, call, JByteArrayRef{env, result});
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    callRpc
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_callRpc
  (JNIEnv* env, jclass, jint entry, jbyteArray params)
{
  if (!params) {
    nullPointerEx.Throw(env, "params cannot be null");
    return 0;
  }
  return nt::CallRpc(entry, JByteArrayRef{env, params});
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getRpcResult
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getRpcResult__II
  (JNIEnv* env, jclass, jint entry, jint call)
{
  std::string result;
  if (!nt::GetRpcResult(entry, call, &result)) return nullptr;
  return MakeJByteArray(env, result);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getRpcResult
 * Signature: (IID)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getRpcResult__IID
  (JNIEnv* env, jclass, jint entry, jint call, jdouble timeout)
{
  std::string result;
  bool timed_out = false;
  if (!nt::GetRpcResult(entry, call, &result, timeout, &timed_out))
    return nullptr;
  return MakeJByteArray(env, result);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    cancelRpcResult
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_cancelRpcResult
  (JNIEnv*, jclass, jint entry, jint call)
{
  nt::CancelRpcResult(entry, call);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getRpc
 * Signature: (I[B)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getRpc
  (JNIEnv* env, jclass, jint entry, jbyteArray defaultValue)
{
  auto val = nt::GetEntryValue(entry);
  if (!val || !val->IsRpc()) return defaultValue;
  return MakeJByteArray(env, val->GetRpc());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setNetworkIdentity
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setNetworkIdentity
  (JNIEnv* env, jclass, jint inst, jstring name)
{
  if (!name) {
    nullPointerEx.Throw(env, "name cannot be null");
    return;
  }
  nt::SetNetworkIdentity(inst, JStringRef{env, name}.str());
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getNetworkMode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getNetworkMode
  (JNIEnv*, jclass, jint inst)
{
  return nt::GetNetworkMode(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    startServer
 * Signature: (ILjava/lang/String;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_startServer
  (JNIEnv* env, jclass, jint inst, jstring persistFilename,
   jstring listenAddress, jint port)
{
  if (!persistFilename) {
    nullPointerEx.Throw(env, "persistFilename cannot be null");
    return;
  }
  if (!listenAddress) {
    nullPointerEx.Throw(env, "listenAddress cannot be null");
    return;
  }
  nt::StartServer(inst, JStringRef{env, persistFilename}.str(),
                  JStringRef{env, listenAddress}.c_str(), port);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    stopServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_stopServer
  (JNIEnv*, jclass, jint inst)
{
  nt::StopServer(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_startClient__I
  (JNIEnv*, jclass, jint inst)
{
  nt::StartClient(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_startClient__ILjava_lang_String_2I
  (JNIEnv* env, jclass, jint inst, jstring serverName, jint port)
{
  if (!serverName) {
    nullPointerEx.Throw(env, "serverName cannot be null");
    return;
  }
  nt::StartClient(inst, JStringRef{env, serverName}.c_str(), port);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: (I[Ljava/lang/Object;[I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_startClient__I_3Ljava_lang_String_2_3I
  (JNIEnv* env, jclass, jint inst, jobjectArray serverNames, jintArray ports)
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
    servers.emplace_back(
        std::make_pair(nt::StringRef(names.back()), portInts[i]));
  }
  env->ReleaseIntArrayElements(ports, portInts, JNI_ABORT);
  nt::StartClient(inst, servers);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    startClientTeam
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_startClientTeam
  (JNIEnv* env, jclass cls, jint inst, jint team, jint port)
{
  nt::StartClientTeam(inst, team, port);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    stopClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_stopClient
  (JNIEnv*, jclass, jint inst)
{
  nt::StopClient(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setServer
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setServer__ILjava_lang_String_2I
  (JNIEnv* env, jclass, jint inst, jstring serverName, jint port)
{
  if (!serverName) {
    nullPointerEx.Throw(env, "serverName cannot be null");
    return;
  }
  nt::SetServer(inst, JStringRef{env, serverName}.c_str(), port);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setServer
 * Signature: (I[Ljava/lang/Object;[I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setServer__I_3Ljava_lang_String_2_3I
  (JNIEnv* env, jclass, jint inst, jobjectArray serverNames, jintArray ports)
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
    servers.emplace_back(
        std::make_pair(nt::StringRef(names.back()), portInts[i]));
  }
  env->ReleaseIntArrayElements(ports, portInts, JNI_ABORT);
  nt::SetServer(inst, servers);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setServerTeam
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setServerTeam
  (JNIEnv* env, jclass, jint inst, jint team, jint port)
{
  nt::SetServerTeam(inst, team, port);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    startDSClient
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_startDSClient
  (JNIEnv*, jclass, jint inst, jint port)
{
  nt::StartDSClient(inst, port);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    stopDSClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_stopDSClient
  (JNIEnv*, jclass, jint inst)
{
  nt::StopDSClient(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    setUpdateRate
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_setUpdateRate
  (JNIEnv*, jclass, jint inst, jdouble interval)
{
  nt::SetUpdateRate(inst, interval);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    flush
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_flush
  (JNIEnv*, jclass, jint inst)
{
  nt::Flush(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    getConnections
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_getConnections
  (JNIEnv* env, jclass, jint inst)
{
  auto arr = nt::GetConnections(inst);
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
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    isConnected
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_isConnected
  (JNIEnv*, jclass, jint inst)
{
  return nt::IsConnected(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    savePersistent
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_savePersistent
  (JNIEnv* env, jclass, jint inst, jstring filename)
{
  if (!filename) {
    nullPointerEx.Throw(env, "filename cannot be null");
    return;
  }
  const char* err = nt::SavePersistent(inst, JStringRef{env, filename}.str());
  if (err) persistentEx.Throw(env, err);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    loadPersistent
 * Signature: (ILjava/lang/String;)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_loadPersistent
  (JNIEnv* env, jclass, jint inst, jstring filename)
{
  if (!filename) {
    nullPointerEx.Throw(env, "filename cannot be null");
    return nullptr;
  }
  std::vector<std::string> warns;
  const char* err = nt::LoadPersistent(inst, JStringRef{env, filename}.str(),
                                       [&](size_t line, const char* msg) {
                                         wpi::SmallString<128> warn;
                                         wpi::raw_svector_ostream oss(warn);
                                         oss << line << ": " << msg;
                                         warns.emplace_back(oss.str());
                                       });
  if (err) {
    persistentEx.Throw(env, err);
    return nullptr;
  }
  return MakeJStringArray(env, warns);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    saveEntries
 * Signature: (ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_saveEntries
  (JNIEnv* env, jclass, jint inst, jstring filename, jstring prefix)
{
  if (!filename) {
    nullPointerEx.Throw(env, "filename cannot be null");
    return;
  }
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return;
  }
  const char* err = nt::SaveEntries(inst, JStringRef{env, filename}.str(),
                                    JStringRef{env, prefix}.str());
  if (err) persistentEx.Throw(env, err);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    loadEntries
 * Signature: (ILjava/lang/String;Ljava/lang/String;)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_loadEntries
  (JNIEnv* env, jclass, jint inst, jstring filename, jstring prefix)
{
  if (!filename) {
    nullPointerEx.Throw(env, "filename cannot be null");
    return nullptr;
  }
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  std::vector<std::string> warns;
  const char* err = nt::LoadEntries(inst, JStringRef{env, filename}.str(),
                                    JStringRef{env, prefix}.str(),
                                    [&](size_t line, const char* msg) {
                                      wpi::SmallString<128> warn;
                                      wpi::raw_svector_ostream oss(warn);
                                      oss << line << ": " << msg;
                                      warns.emplace_back(oss.str());
                                    });
  if (err) {
    persistentEx.Throw(env, err);
    return nullptr;
  }
  return MakeJStringArray(env, warns);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_now
  (JNIEnv*, jclass)
{
  return nt::Now();
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    createLoggerPoller
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_createLoggerPoller
  (JNIEnv*, jclass, jint inst)
{
  return nt::CreateLoggerPoller(inst);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    destroyLoggerPoller
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_destroyLoggerPoller
  (JNIEnv*, jclass, jint poller)
{
  nt::DestroyLoggerPoller(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    addPolledLogger
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_addPolledLogger
  (JNIEnv*, jclass, jint poller, jint minLevel, jint maxLevel)
{
  return nt::AddPolledLogger(poller, minLevel, maxLevel);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollLogger
 * Signature: (Ljava/lang/Object;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollLogger
  (JNIEnv* env, jclass, jobject inst, jint poller)
{
  auto events = nt::PollLogger(poller);
  if (events.empty()) {
    interruptedEx.Throw(env, "PollLogger interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, events);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    pollLoggerTimeout
 * Signature: (Ljava/lang/Object;ID)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_pollLoggerTimeout
  (JNIEnv* env, jclass, jobject inst, jint poller, jdouble timeout)
{
  bool timed_out = false;
  auto events = nt::PollLogger(poller, timeout, &timed_out);
  if (events.empty() && !timed_out) {
    interruptedEx.Throw(env, "PollLogger interrupted");
    return nullptr;
  }
  return MakeJObject(env, inst, events);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    cancelPollLogger
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_cancelPollLogger
  (JNIEnv*, jclass, jint poller)
{
  nt::CancelPollLogger(poller);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    removeLogger
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_removeLogger
  (JNIEnv*, jclass, jint logger)
{
  nt::RemoveLogger(logger);
}

/*
 * Class:     edu_wpi_first_networktables_NetworkTablesJNI
 * Method:    waitForLoggerQueue
 * Signature: (ID)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_networktables_NetworkTablesJNI_waitForLoggerQueue
  (JNIEnv*, jclass, jint inst, jdouble timeout)
{
  return nt::WaitForLoggerQueue(inst, timeout);
}

}  // extern "C"
