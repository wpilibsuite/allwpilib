// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "org_wpilib_networktables_NetworkTablesJNI.h"
#include "wpi/nt/ntcore.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/ConvertUTF.hpp"
#include "wpi/util/jni_util.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::util::java;

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace wpi::nt {
bool JNI_LoadTypes(JNIEnv* env);
void JNI_UnloadTypes(JNIEnv* env);
}  // namespace wpi::nt

//
// Globals and load/unload
//

// Used for callback.
static JClass booleanCls;
static JClass connectionInfoCls;
static JClass doubleCls;
static JClass eventCls;
static JClass floatCls;
static JClass logMessageCls;
static JClass longCls;
static JClass optionalLongCls;
static JClass pubSubOptionsCls;
static JClass timeSyncEventDataCls;
static JClass topicInfoCls;
static JClass valueCls;
static JClass valueEventDataCls;
static JException illegalArgEx;
static JException interruptedEx;
static JException nullPointerEx;

static const JClassInit classes[] = {
    {"java/lang/Boolean", &booleanCls},
    {"org/wpilib/networktables/ConnectionInfo", &connectionInfoCls},
    {"java/lang/Double", &doubleCls},
    {"org/wpilib/networktables/NetworkTableEvent", &eventCls},
    {"java/lang/Float", &floatCls},
    {"org/wpilib/networktables/LogMessage", &logMessageCls},
    {"java/lang/Long", &longCls},
    {"java/util/OptionalLong", &optionalLongCls},
    {"org/wpilib/networktables/PubSubOptions", &pubSubOptionsCls},
    {"org/wpilib/networktables/TimeSyncEventData", &timeSyncEventDataCls},
    {"org/wpilib/networktables/TopicInfo", &topicInfoCls},
    {"org/wpilib/networktables/NetworkTableValue", &valueCls},
    {"org/wpilib/networktables/ValueEventData", &valueEventDataCls}};

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgEx},
    {"java/lang/InterruptedException", &interruptedEx},
    {"java/lang/NullPointerException", &nullPointerEx}};

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
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

  if (!wpi::nt::JNI_LoadTypes(env)) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
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
  wpi::nt::JNI_UnloadTypes(env);
}

}  // extern "C"

//
// Conversions from Java objects to C++
//

static wpi::nt::PubSubOptions FromJavaPubSubOptions(JNIEnv* env,
                                                    jobject joptions) {
  if (!joptions) {
    return {};
  }
#define FIELD(name, sig)                                         \
  static jfieldID name##Field = nullptr;                         \
  if (!name##Field) {                                            \
    name##Field = env->GetFieldID(pubSubOptionsCls, #name, sig); \
  }

  FIELD(pollStorage, "I");
  FIELD(periodic, "D");
  FIELD(excludePublisher, "I");
  FIELD(sendAll, "Z");
  FIELD(topicsOnly, "Z");
  FIELD(keepDuplicates, "Z");
  FIELD(prefixMatch, "Z");
  FIELD(disableRemote, "Z");
  FIELD(disableLocal, "Z");
  FIELD(excludeSelf, "Z");
  FIELD(hidden, "Z");

#undef FIELD

#define FIELD(ctype, jtype, name) \
  .name = static_cast<ctype>(env->Get##jtype##Field(joptions, name##Field))

  return {FIELD(unsigned int, Int, pollStorage),
          FIELD(double, Double, periodic),
          FIELD(NT_Publisher, Int, excludePublisher),
          FIELD(bool, Boolean, sendAll),
          FIELD(bool, Boolean, topicsOnly),
          FIELD(bool, Boolean, keepDuplicates),
          FIELD(bool, Boolean, prefixMatch),
          FIELD(bool, Boolean, disableRemote),
          FIELD(bool, Boolean, disableLocal),
          FIELD(bool, Boolean, excludeSelf),
          FIELD(bool, Boolean, hidden)};

#undef GET
#undef FIELD
}

//
// Conversions from C++ to Java objects
//

static jobject MakeJObject(JNIEnv* env, std::optional<int64_t> value) {
  static jmethodID emptyMethod = nullptr;
  static jmethodID ofMethod = nullptr;
  if (!emptyMethod) {
    emptyMethod = env->GetStaticMethodID(optionalLongCls, "empty",
                                         "()Ljava/util/OptionalLong;");
  }
  if (!ofMethod) {
    ofMethod = env->GetStaticMethodID(optionalLongCls, "of",
                                      "(J)Ljava/util/OptionalLong;");
  }
  if (value) {
    return env->CallStaticObjectMethod(optionalLongCls, ofMethod,
                                       static_cast<jlong>(*value));
  } else {
    return env->CallStaticObjectMethod(optionalLongCls, emptyMethod);
  }
}

static jobject MakeJObject(JNIEnv* env, const wpi::nt::Value& value) {
  static jmethodID booleanConstructor = nullptr;
  static jmethodID doubleConstructor = nullptr;
  static jmethodID floatConstructor = nullptr;
  static jmethodID longConstructor = nullptr;
  if (!booleanConstructor) {
    booleanConstructor = env->GetMethodID(booleanCls, "<init>", "(Z)V");
  }
  if (!doubleConstructor) {
    doubleConstructor = env->GetMethodID(doubleCls, "<init>", "(D)V");
  }
  if (!floatConstructor) {
    floatConstructor = env->GetMethodID(floatCls, "<init>", "(F)V");
  }
  if (!longConstructor) {
    longConstructor = env->GetMethodID(longCls, "<init>", "(J)V");
  }

  switch (value.type()) {
    case NT_BOOLEAN:
      return env->NewObject(booleanCls, booleanConstructor,
                            static_cast<jboolean>(value.GetBoolean() ? 1 : 0));
    case NT_INTEGER:
      return env->NewObject(longCls, longConstructor,
                            static_cast<jlong>(value.GetInteger()));
    case NT_FLOAT:
      return env->NewObject(floatCls, floatConstructor,
                            static_cast<jfloat>(value.GetFloat()));
    case NT_DOUBLE:
      return env->NewObject(doubleCls, doubleConstructor,
                            static_cast<jdouble>(value.GetDouble()));
    case NT_STRING:
      return MakeJString(env, value.GetString());
    case NT_RAW:
      return MakeJByteArray(env, value.GetRaw());
    case NT_BOOLEAN_ARRAY:
      return MakeJBooleanArray(env, value.GetBooleanArray());
    case NT_INTEGER_ARRAY:
      return MakeJLongArray(env, value.GetIntegerArray());
    case NT_FLOAT_ARRAY:
      return MakeJFloatArray(env, value.GetFloatArray());
    case NT_DOUBLE_ARRAY:
      return MakeJDoubleArray(env, value.GetDoubleArray());
    case NT_STRING_ARRAY:
      return MakeJStringArray(env, value.GetStringArray());
    default:
      return nullptr;
  }
}

static jobject MakeJValue(JNIEnv* env, const wpi::nt::Value& value) {
  static jmethodID constructor =
      env->GetMethodID(valueCls, "<init>", "(ILjava/lang/Object;JJ)V");
  if (!value) {
    return env->NewObject(valueCls, constructor,
                          static_cast<jint>(NT_UNASSIGNED), nullptr,
                          static_cast<jlong>(0), static_cast<jlong>(0));
  }
  return env->NewObject(valueCls, constructor, static_cast<jint>(value.type()),
                        MakeJObject(env, value),
                        static_cast<jlong>(value.time()),
                        static_cast<jlong>(value.server_time()));
}

static jobject MakeJObject(JNIEnv* env, const wpi::nt::ConnectionInfo& info) {
  static jmethodID constructor =
      env->GetMethodID(connectionInfoCls, "<init>",
                       "(Ljava/lang/String;Ljava/lang/String;IJI)V");
  JLocal<jstring> remote_id{env, MakeJString(env, info.remote_id)};
  JLocal<jstring> remote_ip{env, MakeJString(env, info.remote_ip)};
  return env->NewObject(connectionInfoCls, constructor, remote_id.obj(),
                        remote_ip.obj(), static_cast<jint>(info.remote_port),
                        static_cast<jlong>(info.last_update),
                        static_cast<jint>(info.protocol_version));
}

static jobject MakeJObject(JNIEnv* env, const wpi::nt::LogMessage& msg) {
  static jmethodID constructor = env->GetMethodID(
      logMessageCls, "<init>", "(ILjava/lang/String;ILjava/lang/String;)V");
  JLocal<jstring> filename{env, MakeJString(env, msg.filename)};
  JLocal<jstring> message{env, MakeJString(env, msg.message)};
  return env->NewObject(logMessageCls, constructor,
                        static_cast<jint>(msg.level), filename.obj(),
                        static_cast<jint>(msg.line), message.obj());
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const wpi::nt::TopicInfo& info) {
  static jmethodID constructor = env->GetMethodID(
      topicInfoCls, "<init>",
      "(Lorg/wpilib/networktables/"
      "NetworkTableInstance;ILjava/lang/String;ILjava/lang/String;)V");
  JLocal<jstring> name{env, MakeJString(env, info.name)};
  JLocal<jstring> typeStr{env, MakeJString(env, info.type_str)};
  return env->NewObject(topicInfoCls, constructor, inst,
                        static_cast<jint>(info.topic), name.obj(),
                        static_cast<jint>(info.type), typeStr.obj());
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const wpi::nt::ValueEventData& data) {
  static jmethodID constructor =
      env->GetMethodID(valueEventDataCls, "<init>",
                       "(Lorg/wpilib/networktables/NetworkTableInstance;II"
                       "Lorg/wpilib/networktables/NetworkTableValue;)V");
  JLocal<jobject> value{env, MakeJValue(env, data.value)};
  return env->NewObject(valueEventDataCls, constructor, inst,
                        static_cast<jint>(data.topic),
                        static_cast<jint>(data.subentry), value.obj());
}

static jobject MakeJObject(JNIEnv* env,
                           const wpi::nt::TimeSyncEventData& data) {
  static jmethodID constructor =
      env->GetMethodID(timeSyncEventDataCls, "<init>", "(JJZ)V");
  return env->NewObject(timeSyncEventDataCls, constructor,
                        static_cast<jlong>(data.serverTimeOffset),
                        static_cast<jlong>(data.rtt2),
                        static_cast<jboolean>(data.valid));
}

static jobject MakeJObject(JNIEnv* env, jobject inst,
                           const wpi::nt::Event& event) {
  static jmethodID constructor =
      env->GetMethodID(eventCls, "<init>",
                       "(Lorg/wpilib/networktables/NetworkTableInstance;II"
                       "Lorg/wpilib/networktables/ConnectionInfo;"
                       "Lorg/wpilib/networktables/TopicInfo;"
                       "Lorg/wpilib/networktables/ValueEventData;"
                       "Lorg/wpilib/networktables/LogMessage;"
                       "Lorg/wpilib/networktables/TimeSyncEventData;)V");
  JLocal<jobject> connInfo{env, nullptr};
  JLocal<jobject> topicInfo{env, nullptr};
  JLocal<jobject> valueData{env, nullptr};
  JLocal<jobject> logMessage{env, nullptr};
  JLocal<jobject> timeSyncData{env, nullptr};
  if (auto v = event.GetConnectionInfo()) {
    connInfo = JLocal<jobject>{env, MakeJObject(env, *v)};
  } else if (auto v = event.GetTopicInfo()) {
    topicInfo = JLocal<jobject>{env, MakeJObject(env, inst, *v)};
  } else if (auto v = event.GetValueEventData()) {
    valueData = JLocal<jobject>{env, MakeJObject(env, inst, *v)};
  } else if (auto v = event.GetLogMessage()) {
    logMessage = JLocal<jobject>{env, MakeJObject(env, *v)};
  } else if (auto v = event.GetTimeSyncEventData()) {
    timeSyncData = JLocal<jobject>{env, MakeJObject(env, *v)};
  }
  return env->NewObject(
      eventCls, constructor, inst, static_cast<jint>(event.listener),
      static_cast<jint>(event.flags), connInfo.obj(), topicInfo.obj(),
      valueData.obj(), logMessage.obj(), timeSyncData.obj());
}

static jobjectArray MakeJObject(JNIEnv* env,
                                std::span<const wpi::nt::Value> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), valueCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJValue(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobjectArray MakeJObject(JNIEnv* env, jobject inst,
                                std::span<const wpi::nt::Event> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), eventCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, inst, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

extern "C" {

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getDefaultInstance
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getDefaultInstance
  (JNIEnv*, jclass)
{
  return wpi::nt::GetDefaultInstance();
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    createInstance
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_createInstance
  (JNIEnv*, jclass)
{
  return wpi::nt::CreateInstance();
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    destroyInstance
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_destroyInstance
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::DestroyInstance(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getInstanceFromHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getInstanceFromHandle
  (JNIEnv*, jclass, jint handle)
{
  return wpi::nt::GetInstanceFromHandle(handle);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getEntry
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getEntry
  (JNIEnv* env, jclass, jint inst, jstring key)
{
  if (!key) {
    nullPointerEx.Throw(env, "key cannot be null");
    return false;
  }
  return wpi::nt::GetEntry(inst, JStringRef{env, key});
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getEntryName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getEntryName
  (JNIEnv* env, jclass, jint entry)
{
  return MakeJString(env, wpi::nt::GetEntryName(entry));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getEntryLastChange
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getEntryLastChange
  (JNIEnv*, jclass, jint entry)
{
  return wpi::nt::GetEntryLastChange(entry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getType
  (JNIEnv*, jclass, jint entry)
{
  return wpi::nt::GetEntryType(entry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopics
 * Signature: (ILjava/lang/String;I)[I
 */
JNIEXPORT jintArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopics
  (JNIEnv* env, jclass, jint inst, jstring prefix, jint types)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  auto arr = wpi::nt::GetTopics(inst, JStringRef{env, prefix}.str(), types);
  return MakeJIntArray(env, arr);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicsStr
 * Signature: (ILjava/lang/String;[Ljava/lang/Object;)[I
 */
JNIEXPORT jintArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicsStr
  (JNIEnv* env, jclass, jint inst, jstring prefix, jobjectArray types)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  if (!types) {
    nullPointerEx.Throw(env, "types cannot be null");
    return nullptr;
  }

  int len = env->GetArrayLength(types);
  std::vector<std::string> typeStrData;
  std::vector<std::string_view> typeStrs;
  typeStrs.reserve(len);
  for (int i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(types, i))};
    if (!elem) {
      nullPointerEx.Throw(env, "null string in types");
      return nullptr;
    }
    typeStrData.emplace_back(JStringRef{env, elem}.str());
    typeStrs.emplace_back(typeStrData.back());
  }

  auto arr = wpi::nt::GetTopics(inst, JStringRef{env, prefix}.str(), typeStrs);
  return MakeJIntArray(env, arr);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicInfos
 * Signature: (Ljava/lang/Object;ILjava/lang/String;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicInfos
  (JNIEnv* env, jclass, jobject instObject, jint inst, jstring prefix,
   jint types)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  auto arr = wpi::nt::GetTopicInfo(inst, JStringRef{env, prefix}.str(), types);
  jobjectArray jarr = env->NewObjectArray(arr.size(), topicInfoCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> jelem{env, MakeJObject(env, instObject, arr[i])};
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicInfosStr
 * Signature: (Ljava/lang/Object;ILjava/lang/String;[Ljava/lang/Object;)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicInfosStr
  (JNIEnv* env, jclass, jobject instObject, jint inst, jstring prefix,
   jobjectArray types)
{
  if (!prefix) {
    nullPointerEx.Throw(env, "prefix cannot be null");
    return nullptr;
  }
  if (!types) {
    nullPointerEx.Throw(env, "types cannot be null");
    return nullptr;
  }

  int len = env->GetArrayLength(types);
  std::vector<std::string> typeStrData;
  std::vector<std::string_view> typeStrs;
  typeStrs.reserve(len);
  for (int i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(types, i))};
    if (!elem) {
      nullPointerEx.Throw(env, "null string in types");
      return nullptr;
    }
    typeStrData.emplace_back(JStringRef{env, elem}.str());
    typeStrs.emplace_back(typeStrData.back());
  }

  auto arr =
      wpi::nt::GetTopicInfo(inst, JStringRef{env, prefix}.str(), typeStrs);
  jobjectArray jarr = env->NewObjectArray(arr.size(), topicInfoCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> jelem{env, MakeJObject(env, instObject, arr[i])};
    env->SetObjectArrayElement(jarr, i, jelem);
  }
  return jarr;
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopic
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopic
  (JNIEnv* env, jclass, jint inst, jstring name)
{
  return wpi::nt::GetTopic(inst, JStringRef{env, name});
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicName
  (JNIEnv* env, jclass, jint topic)
{
  return MakeJString(env, wpi::nt::GetTopicName(topic));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicType
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicType
  (JNIEnv*, jclass, jint topic)
{
  return wpi::nt::GetTopicType(topic);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setTopicPersistent
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setTopicPersistent
  (JNIEnv*, jclass, jint topic, jboolean value)
{
  wpi::nt::SetTopicPersistent(topic, value);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicPersistent
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicPersistent
  (JNIEnv*, jclass, jint topic)
{
  return wpi::nt::GetTopicPersistent(topic);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setTopicRetained
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setTopicRetained
  (JNIEnv*, jclass, jint topic, jboolean value)
{
  wpi::nt::SetTopicRetained(topic, value);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicRetained
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicRetained
  (JNIEnv*, jclass, jint topic)
{
  return wpi::nt::GetTopicRetained(topic);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setTopicCached
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setTopicCached
  (JNIEnv*, jclass, jint topic, jboolean value)
{
  wpi::nt::SetTopicCached(topic, value);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicCached
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicCached
  (JNIEnv*, jclass, jint topic)
{
  return wpi::nt::GetTopicCached(topic);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicTypeString
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicTypeString
  (JNIEnv* env, jclass, jint topic)
{
  return MakeJString(env, wpi::nt::GetTopicTypeString(topic));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicExists
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicExists
  (JNIEnv*, jclass, jint topic)
{
  return wpi::nt::GetTopicExists(topic);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicProperty
 * Signature: (ILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicProperty
  (JNIEnv* env, jclass, jint topic, jstring name)
{
  return MakeJString(
      env, wpi::nt::GetTopicProperty(topic, JStringRef{env, name}).dump());
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setTopicProperty
 * Signature: (ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setTopicProperty
  (JNIEnv* env, jclass, jint topic, jstring name, jstring value)
{
  wpi::util::json j;
  try {
    j = wpi::util::json::parse(std::string_view{JStringRef{env, value}});
  } catch (wpi::util::json::parse_error& err) {
    illegalArgEx.Throw(
        env, fmt::format("could not parse value JSON: {}", err.what()));
    return;
  }
  wpi::nt::SetTopicProperty(topic, JStringRef{env, name}, j);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    deleteTopicProperty
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_deleteTopicProperty
  (JNIEnv* env, jclass, jint topic, jstring name)
{
  wpi::nt::DeleteTopicProperty(topic, JStringRef{env, name});
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicProperties
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicProperties
  (JNIEnv* env, jclass, jint topic)
{
  return MakeJString(env, wpi::nt::GetTopicProperties(topic).dump());
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setTopicProperties
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setTopicProperties
  (JNIEnv* env, jclass, jint topic, jstring properties)
{
  wpi::util::json j;
  try {
    j = wpi::util::json::parse(std::string_view{JStringRef{env, properties}});
  } catch (wpi::util::json::parse_error& err) {
    illegalArgEx.Throw(
        env, fmt::format("could not parse properties JSON: {}", err.what()));
    return;
  }
  if (!j.is_object()) {
    illegalArgEx.Throw(env, "properties is not a JSON object");
    return;
  }
  wpi::nt::SetTopicProperties(topic, j);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    subscribe
 * Signature: (IILjava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_subscribe
  (JNIEnv* env, jclass, jint topic, jint type, jstring typeStr, jobject options)
{
  return wpi::nt::Subscribe(topic, static_cast<NT_Type>(type),
                            JStringRef{env, typeStr},
                            FromJavaPubSubOptions(env, options));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    unsubscribe
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_unsubscribe
  (JNIEnv*, jclass, jint sub)
{
  wpi::nt::Unsubscribe(sub);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    publish
 * Signature: (IILjava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_publish
  (JNIEnv* env, jclass, jint topic, jint type, jstring typeStr, jobject options)
{
  return wpi::nt::Publish(topic, static_cast<NT_Type>(type),
                          JStringRef{env, typeStr},
                          FromJavaPubSubOptions(env, options));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    publishEx
 * Signature: (IILjava/lang/String;Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_publishEx
  (JNIEnv* env, jclass, jint topic, jint type, jstring typeStr,
   jstring properties, jobject options)
{
  wpi::util::json j;
  try {
    j = wpi::util::json::parse(std::string_view{JStringRef{env, properties}});
  } catch (wpi::util::json::parse_error& err) {
    illegalArgEx.Throw(
        env, fmt::format("could not parse properties JSON: {}", err.what()));
    return 0;
  }
  if (!j.is_object()) {
    illegalArgEx.Throw(env, "properties is not a JSON object");
    return 0;
  }
  return wpi::nt::PublishEx(topic, static_cast<NT_Type>(type),
                            JStringRef{env, typeStr}, j,
                            FromJavaPubSubOptions(env, options));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    unpublish
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_unpublish
  (JNIEnv*, jclass, jint pubentry)
{
  wpi::nt::Unpublish(pubentry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getEntryImpl
 * Signature: (IILjava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getEntryImpl
  (JNIEnv* env, jclass, jint topic, jint type, jstring typeStr, jobject options)
{
  return wpi::nt::GetEntry(topic, static_cast<NT_Type>(type),
                           JStringRef{env, typeStr},
                           FromJavaPubSubOptions(env, options));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    releaseEntry
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_releaseEntry
  (JNIEnv*, jclass, jint entry)
{
  wpi::nt::ReleaseEntry(entry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    release
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_release
  (JNIEnv*, jclass, jint pubsubentry)
{
  wpi::nt::Release(pubsubentry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicFromHandle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicFromHandle
  (JNIEnv*, jclass, jint pubsubentry)
{
  return wpi::nt::GetTopicFromHandle(pubsubentry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    subscribeMultiple
 * Signature: (I[Ljava/lang/Object;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_subscribeMultiple
  (JNIEnv* env, jclass, jint inst, jobjectArray prefixes, jobject options)
{
  if (!prefixes) {
    nullPointerEx.Throw(env, "prefixes cannot be null");
    return {};
  }
  int len = env->GetArrayLength(prefixes);

  std::vector<std::string> prefixStrings;
  std::vector<std::string_view> prefixStringViews;
  prefixStrings.reserve(len);
  prefixStringViews.reserve(len);
  for (int i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(prefixes, i))};
    if (!elem) {
      nullPointerEx.Throw(env, "null string in prefixes");
      return {};
    }
    prefixStrings.emplace_back(JStringRef{env, elem}.str());
    prefixStringViews.emplace_back(prefixStrings.back());
  }

  return wpi::nt::SubscribeMultiple(inst, prefixStringViews,
                                    FromJavaPubSubOptions(env, options));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    unsubscribeMultiple
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_unsubscribeMultiple
  (JNIEnv*, jclass, jint sub)
{
  wpi::nt::UnsubscribeMultiple(sub);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    readQueueValue
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_readQueueValue
  (JNIEnv* env, jclass, jint subentry)
{
  return MakeJObject(env, wpi::nt::ReadQueueValue(subentry));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getValue
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getValue
  (JNIEnv* env, jclass, jint entry)
{
  return MakeJValue(env, wpi::nt::GetEntryValue(entry));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setEntryFlags
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setEntryFlags
  (JNIEnv*, jclass, jint entry, jint flags)
{
  wpi::nt::SetEntryFlags(entry, flags);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getEntryFlags
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getEntryFlags
  (JNIEnv*, jclass, jint entry)
{
  return wpi::nt::GetEntryFlags(entry);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getTopicInfo
 * Signature: (Ljava/lang/Object;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getTopicInfo
  (JNIEnv* env, jclass, jobject inst, jint topic)
{
  return MakeJObject(env, inst, wpi::nt::GetTopicInfo(topic));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    createListenerPoller
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_createListenerPoller
  (JNIEnv*, jclass, jint inst)
{
  return wpi::nt::CreateListenerPoller(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    destroyListenerPoller
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_destroyListenerPoller
  (JNIEnv*, jclass, jint poller)
{
  wpi::nt::DestroyListenerPoller(poller);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    addListener
 * Signature: (I[Ljava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_addListener__I_3Ljava_lang_String_2I
  (JNIEnv* env, jclass, jint poller, jobjectArray prefixes, jint flags)
{
  if (!prefixes) {
    nullPointerEx.Throw(env, "prefixes cannot be null");
    return 0;
  }

  size_t len = env->GetArrayLength(prefixes);
  std::vector<std::string> arr;
  std::vector<std::string_view> arrview;
  arr.reserve(len);
  arrview.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(prefixes, i))};
    if (!elem) {
      nullPointerEx.Throw(env, "prefixes cannot contain null");
      return 0;
    }
    arr.emplace_back(JStringRef{env, elem}.str());
    // this is safe because of the reserve (so arr elements won't move)
    arrview.emplace_back(arr.back());
  }

  return wpi::nt::AddPolledListener(poller, arrview, flags);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    addListener
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_addListener__III
  (JNIEnv* env, jclass, jint poller, jint handle, jint flags)
{
  return wpi::nt::AddPolledListener(poller, handle, flags);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    readListenerQueue
 * Signature: (Ljava/lang/Object;I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_readListenerQueue
  (JNIEnv* env, jclass, jobject inst, jint poller)
{
  return MakeJObject(env, inst, wpi::nt::ReadListenerQueue(poller));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    removeListener
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_removeListener
  (JNIEnv*, jclass, jint topicListener)
{
  wpi::nt::RemoveListener(topicListener);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getNetworkMode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getNetworkMode
  (JNIEnv*, jclass, jint inst)
{
  return wpi::nt::GetNetworkMode(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    startLocal
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_startLocal
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::StartLocal(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    stopLocal
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_stopLocal
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::StopLocal(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    startServer
 * Signature: (ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_startServer
  (JNIEnv* env, jclass, jint inst, jstring persistFilename,
   jstring listenAddress, jstring mdnsService, jint port)
{
  if (!persistFilename) {
    nullPointerEx.Throw(env, "persistFilename cannot be null");
    return;
  }
  if (!listenAddress) {
    nullPointerEx.Throw(env, "listenAddress cannot be null");
    return;
  }
  if (!mdnsService) {
    nullPointerEx.Throw(env, "mdnsService cannot be null");
    return;
  }
  wpi::nt::StartServer(inst, JStringRef{env, persistFilename}.str(),
                       JStringRef{env, listenAddress}.c_str(),
                       JStringRef{env, mdnsService}.c_str(), port);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    stopServer
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_stopServer
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::StopServer(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    startClient
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_startClient
  (JNIEnv* env, jclass, jint inst, jstring identity)
{
  if (!identity) {
    nullPointerEx.Throw(env, "identity cannot be null");
    return;
  }
  wpi::nt::StartClient(inst, JStringRef{env, identity}.str());
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    stopClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_stopClient
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::StopClient(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setServer
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setServer__ILjava_lang_String_2I
  (JNIEnv* env, jclass, jint inst, jstring serverName, jint port)
{
  if (!serverName) {
    nullPointerEx.Throw(env, "serverName cannot be null");
    return;
  }
  wpi::nt::SetServer(inst, JStringRef{env, serverName}.c_str(), port);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setServer
 * Signature: (I[Ljava/lang/Object;[I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setServer__I_3Ljava_lang_String_2_3I
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
  JSpan<const jint> portInts{env, ports};
  if (!portInts) {
    return;
  }

  std::vector<std::string> names;
  std::vector<std::pair<std::string_view, unsigned int>> servers;
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
        std::pair{std::string_view{names.back()}, portInts[i]});
  }
  wpi::nt::SetServer(inst, servers);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    setServerTeam
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_setServerTeam
  (JNIEnv* env, jclass, jint inst, jint team, jint port)
{
  wpi::nt::SetServerTeam(inst, team, port);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    disconnect
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_disconnect
  (JNIEnv* env, jclass, jint inst)
{
  wpi::nt::Disconnect(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    startDSClient
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_startDSClient
  (JNIEnv*, jclass, jint inst, jint port)
{
  wpi::nt::StartDSClient(inst, port);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    stopDSClient
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_stopDSClient
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::StopDSClient(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    flushLocal
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_flushLocal
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::FlushLocal(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    flush
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_flush
  (JNIEnv*, jclass, jint inst)
{
  wpi::nt::Flush(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getConnections
 * Signature: (I)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getConnections
  (JNIEnv* env, jclass, jint inst)
{
  auto arr = wpi::nt::GetConnections(inst);
  jobjectArray jarr =
      env->NewObjectArray(arr.size(), connectionInfoCls, nullptr);
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
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    isConnected
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_isConnected
  (JNIEnv*, jclass, jint inst)
{
  return wpi::nt::IsConnected(inst);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    getServerTimeOffset
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_getServerTimeOffset
  (JNIEnv* env, jclass, jint inst)
{
  return MakeJObject(env, wpi::nt::GetServerTimeOffset(inst));
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_now
  (JNIEnv*, jclass)
{
  return wpi::nt::Now();
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    startEntryDataLog
 * Signature: (IJLjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_startEntryDataLog
  (JNIEnv* env, jclass, jint inst, jlong log, jstring prefix, jstring logPrefix)
{
  return wpi::nt::StartEntryDataLog(
      inst, *reinterpret_cast<wpi::log::DataLog*>(log), JStringRef{env, prefix},
      JStringRef{env, logPrefix});
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    stopEntryDataLog
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_stopEntryDataLog
  (JNIEnv*, jclass, jint logger)
{
  wpi::nt::StopEntryDataLog(logger);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    startConnectionDataLog
 * Signature: (IJLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_startConnectionDataLog
  (JNIEnv* env, jclass, jint inst, jlong log, jstring name)
{
  return wpi::nt::StartConnectionDataLog(
      inst, *reinterpret_cast<wpi::log::DataLog*>(log), JStringRef{env, name});
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    stopConnectionDataLog
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_stopConnectionDataLog
  (JNIEnv*, jclass, jint logger)
{
  wpi::nt::StopConnectionDataLog(logger);
}

/*
 * Class:     org_wpilib_networktables_NetworkTablesJNI
 * Method:    addLogger
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_networktables_NetworkTablesJNI_addLogger
  (JNIEnv*, jclass, jint poller, jint minLevel, jint maxLevel)
{
  return wpi::nt::AddPolledLogger(poller, minLevel, maxLevel);
}

}  // extern "C"
