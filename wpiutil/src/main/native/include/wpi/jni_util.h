// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_JNI_UTIL_H_
#define WPIUTIL_WPI_JNI_UTIL_H_

#include <jni.h>

#include <concepts>
#include <queue>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include "wpi/ConvertUTF.h"
#include "wpi/SafeThread.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringExtras.h"
#include "wpi/mutex.h"
#include "wpi/raw_ostream.h"

/** Java Native Interface (JNI) utility functions */
namespace wpi::java {

/**
 * Gets a Java stack trace.
 *
 * Also provides the last function in the stack trace not starting with
 * excludeFuncPrefix (useful for e.g. finding the first user call to a series of
 * library functions).
 *
 * @param env JRE environment.
 * @param func Storage for last function in the stack trace not starting with
 *             excludeFuncPrefix.
 * @param excludeFuncPrefix Prefix for functions to ignore in stack trace.
 */
std::string GetJavaStackTrace(JNIEnv* env, std::string* func = nullptr,
                              std::string_view excludeFuncPrefix = {});

/**
 * Finds a class and keeps it as a global reference.
 *
 * Use with caution, as the destructor does NOT call DeleteGlobalRef due to
 * potential shutdown issues with doing so.
 */
class JClass {
 public:
  JClass() = default;

  JClass(JNIEnv* env, const char* name) {
    jclass local = env->FindClass(name);
    if (!local) {
      return;
    }
    m_cls = static_cast<jclass>(env->NewGlobalRef(local));
    env->DeleteLocalRef(local);
  }

  void free(JNIEnv* env) {
    if (m_cls) {
      env->DeleteGlobalRef(m_cls);
    }
    m_cls = nullptr;
  }

  explicit operator bool() const { return m_cls; }

  operator jclass() const { return m_cls; }

 protected:
  jclass m_cls = nullptr;
};

struct JClassInit {
  const char* name;
  JClass* cls;
};

template <typename T>
class JGlobal {
 public:
  JGlobal() = default;

  JGlobal(JNIEnv* env, T obj) {
    m_cls = static_cast<T>(env->NewGlobalRef(obj));
  }

  void free(JNIEnv* env) {
    if (m_cls) {
      env->DeleteGlobalRef(m_cls);
    }
    m_cls = nullptr;
  }

  explicit operator bool() const { return m_cls; }

  operator T() const { return m_cls; }  // NOLINT

 protected:
  T m_cls = nullptr;
};

/**
 * Container class for cleaning up Java local references.
 *
 * The destructor calls DeleteLocalRef.
 */
template <typename T>
class JLocal {
 public:
  JLocal(JNIEnv* env, T obj) : m_env(env), m_obj(obj) {}
  JLocal(const JLocal&) = delete;
  JLocal(JLocal&& oth) : m_env(oth.m_env), m_obj(oth.m_obj) {
    oth.m_obj = nullptr;
  }
  JLocal& operator=(const JLocal&) = delete;
  JLocal& operator=(JLocal&& oth) {
    m_env = oth.m_env;
    m_obj = oth.m_obj;
    oth.m_obj = nullptr;
    return *this;
  }
  ~JLocal() {
    if (m_obj) {
      m_env->DeleteLocalRef(m_obj);
    }
  }
  operator T() { return m_obj; }  // NOLINT
  T obj() { return m_obj; }

 private:
  JNIEnv* m_env;
  T m_obj;
};

//
// Conversions from Java objects to C++
//

/**
 * Java string (jstring) reference.
 *
 * The string is provided as UTF8. This is not actually a reference, as it makes
 * a copy of the string characters, but it's named this way for consistency.
 */
class JStringRef {
 public:
  JStringRef(JNIEnv* env, jstring str) {
    if (str) {
      jsize size = env->GetStringLength(str);
      const jchar* chars = env->GetStringCritical(str, nullptr);
      if (chars) {
        convertUTF16ToUTF8String(std::span<const jchar>(chars, size), m_str);
        env->ReleaseStringCritical(str, chars);
      }
    } else {
      fmt::print(stderr, "JStringRef was passed a null pointer at\n",
                 GetJavaStackTrace(env));
    }
    // Ensure str is null-terminated.
    m_str.push_back('\0');
    m_str.pop_back();
  }

  operator std::string_view() const { return m_str.str(); }  // NOLINT
  std::string_view str() const { return m_str.str(); }
  const char* c_str() const { return m_str.data(); }
  size_t size() const { return m_str.size(); }

 private:
  SmallString<128> m_str;
};

namespace detail {

template <typename T>
struct ArrayHelper {};

#define WPI_JNI_ARRAYHELPER(T, F)                                             \
  template <>                                                                 \
  struct ArrayHelper<T> {                                                     \
    using jarray_type = T##Array;                                             \
    static T* GetArrayElements(JNIEnv* env, jarray_type jarr) {               \
      return env->Get##F##ArrayElements(jarr, nullptr);                       \
    }                                                                         \
    static void ReleaseArrayElements(JNIEnv* env, jarray_type jarr, T* elems, \
                                     jint mode) {                             \
      env->Release##F##ArrayElements(jarr, elems, mode);                      \
    }                                                                         \
  };

WPI_JNI_ARRAYHELPER(jboolean, Boolean)
WPI_JNI_ARRAYHELPER(jbyte, Byte)
WPI_JNI_ARRAYHELPER(jshort, Short)
WPI_JNI_ARRAYHELPER(jint, Int)
WPI_JNI_ARRAYHELPER(jlong, Long)
WPI_JNI_ARRAYHELPER(jfloat, Float)
WPI_JNI_ARRAYHELPER(jdouble, Double)

#undef WPI_JNI_ARRAYHELPER

template <typename T>
concept JArrayType =
    requires { typename ArrayHelper<std::remove_cv_t<T>>::jarray_type; };

template <typename CvSrc, typename Dest>
struct copy_cv {
 private:
  using U0 = std::remove_cv_t<Dest>;
  using U1 = std::conditional_t<std::is_const_v<CvSrc>, const U0, U0>;
  using U2 = std::conditional_t<std::is_volatile_v<CvSrc>, volatile U1, U1>;

 public:
  using type = U2;
};

template <typename CvSrc, typename Dest>
using copy_cv_t = typename copy_cv<CvSrc, Dest>::type;

template <typename From, typename To>
constexpr bool is_qualification_convertible_v =
    !(std::is_const_v<From> && !std::is_const_v<To>)&&!(
        std::is_volatile_v<From> && !std::is_volatile_v<To>);

/**
 * Helper class for working with JNI arrays.
 *
 * This class exposes an is_valid() member and an explicit conversion to bool
 * which indicate if the span is valid. Operations on invalid spans are
 * undefined.
 *
 * Note that Set<PrimitiveType>ArrayRegion may be faster for pure writes since
 * it avoids copying the elements from Java to C++.
 *
 * @tparam T The element type of the array (e.g., jdouble).
 * @tparam IsCritical If true, Get/ReleasePrimitiveArrayCritical will be used
 * instead of Get/Release\<PrimitiveType\>ArrayElements.
 * @tparam Size The number of elements in the span.
 */
template <JArrayType T, bool IsCritical, size_t Size = std::dynamic_extent>
class JSpanBase {
  using ArrHelper = ArrayHelper<std::remove_cv_t<T>>;
  using jarray_type = typename ArrHelper::jarray_type;

 public:
  JSpanBase(const JSpanBase&) = delete;
  JSpanBase& operator=(const JSpanBase&) = delete;

  JSpanBase(JSpanBase&& other)
      : m_valid{other.m_valid},
        m_env{other.m_env},
        m_jarr{other.m_jarr},
        m_size{other.m_size},
        m_elements{other.m_elements} {
    other.m_jarr = nullptr;
    other.m_elements = nullptr;
  }

  JSpanBase& operator=(JSpanBase&& other) {
    m_valid = other.m_valid;
    m_env = other.m_env;
    m_jarr = other.m_jarr;
    m_size = other.m_size;
    m_elements = other.m_elements;
    other.m_valid = false;
    other.m_jarr = nullptr;
    other.m_elements = nullptr;
    return *this;
  }

  JSpanBase(JNIEnv* env, jobject bb, size_t size)
    requires(!IsCritical)
      : m_valid{Size == std::dynamic_extent || size == Size},
        m_env{env},
        m_jarr{nullptr},
        m_size{size},
        m_elements{static_cast<std::remove_cv_t<T>*>(
            bb ? env->GetDirectBufferAddress(bb) : nullptr)} {
    if (!bb) {
      fmt::print(stderr, "JSpan was passed a null pointer at\n",
                 GetJavaStackTrace(env));
    }
  }

  JSpanBase(JNIEnv* env, jarray_type jarr, size_t size)
      : m_valid{Size == std::dynamic_extent || size == Size},
        m_env{env},
        m_jarr{jarr},
        m_size{size},
        m_elements{nullptr} {
    if (jarr) {
      if constexpr (IsCritical) {
        m_elements = static_cast<std::remove_cv_t<T>*>(
            env->GetPrimitiveArrayCritical(jarr, nullptr));
      } else {
        m_elements = ArrHelper::GetArrayElements(env, jarr);
      }
    } else {
      fmt::print(stderr, "JSpan was passed a null pointer at\n",
                 GetJavaStackTrace(env));
    }
  }

  JSpanBase(JNIEnv* env, jarray_type jarr)
      : JSpanBase(env, jarr, jarr ? env->GetArrayLength(jarr) : 0) {}

  ~JSpanBase() {
    if (m_jarr && m_elements) {
      constexpr jint mode = std::is_const_v<T> ? JNI_ABORT : 0;
      if constexpr (IsCritical) {
        m_env->ReleasePrimitiveArrayCritical(m_jarr, m_elements, mode);
      } else {
        ArrHelper::ReleaseArrayElements(m_env, m_jarr, m_elements, mode);
      }
    }
  }

  operator std::span<T, Size>() const { return array(); }

  std::span<T, Size> array() const {
    // If Size is dynamic_extent, can return empty span
    // Unfortunately, sized spans will return a span over nullptr if m_elements
    // is nullptr
    if constexpr (Size == std::dynamic_extent) {
      if (!m_elements) {
        return {};
      }
    }
    return std::span<T, Size>{m_elements, m_size};
  }

  T* begin() const { return m_elements; }

  T* end() const { return m_elements + m_size; }

  bool is_valid() const { return m_valid && m_elements != nullptr; }

  explicit operator bool() const { return is_valid(); }

  T* data() const { return m_elements; }

  size_t size() const { return m_size; }

  const T& operator[](size_t i) const { return m_elements[i]; }

  T& operator[](size_t i)
    requires(!std::is_const_v<T>)
  {
    return m_elements[i];
  }

  // Provide std::string_view and span<const uint8_t> conversions for jbyte

  operator std::string_view() const
    requires std::is_same_v<std::remove_cv_t<T>, jbyte>
  {
    return str();
  }

  std::string_view str() const
    requires std::is_same_v<std::remove_cv_t<T>, jbyte>
  {
    auto arr = array();
    if (arr.empty()) {
      return {};
    }
    return {reinterpret_cast<const char*>(arr.data()), arr.size()};
  }

  std::span<copy_cv_t<T, uint8_t>, Size> uarray() const
    requires std::is_same_v<std::remove_cv_t<T>, jbyte>
  {
    auto arr = array();
    if (arr.empty()) {
      return {};
    }
    return {reinterpret_cast<const uint8_t*>(arr.data()), arr.size()};
  }

  // Support both "long long" and "long" on 64-bit systems

  template <typename U>
    requires(sizeof(U) == sizeof(jlong) && std::integral<U> &&
             is_qualification_convertible_v<T, U>)
  operator std::span<U, Size>() const
    requires std::is_same_v<std::remove_cv_t<T>, jlong>
  {
    auto arr = array();
    if (arr.empty()) {
      return {};
    }
    return {reinterpret_cast<U*>(arr.data()), arr.size()};
  }

 private:
  bool m_valid;
  JNIEnv* m_env;
  jarray_type m_jarr = nullptr;
  size_t m_size;
  std::remove_cv_t<T>* m_elements;
};

}  // namespace detail

template <typename T, size_t Extent = std::dynamic_extent>
using JSpan = detail::JSpanBase<T, false, Extent>;

template <typename T, size_t Extent = std::dynamic_extent>
using CriticalJSpan = detail::JSpanBase<T, true, Extent>;

//
// Conversions from C++ to Java objects
//

/**
 * Convert a UTF8 string into a jstring.
 *
 * @param env JRE environment.
 * @param str String to convert.
 */
inline jstring MakeJString(JNIEnv* env, std::string_view str) {
  SmallVector<UTF16, 128> chars;
  convertUTF8ToUTF16String(str, chars);
  return env->NewString(chars.begin(), chars.size());
}

// details for MakeJIntArray
namespace detail {

template <typename T>
struct ConvertIntArray {
  static jintArray ToJava(JNIEnv* env, std::span<const T> arr) {
    if constexpr (sizeof(T) == sizeof(jint) && std::integral<T>) {
      // Fast path (use SetIntArrayRegion).
      jintArray jarr = env->NewIntArray(arr.size());
      if (!jarr) {
        return nullptr;
      }
      env->SetIntArrayRegion(jarr, 0, arr.size(),
                             reinterpret_cast<const jint*>(arr.data()));
      return jarr;
    } else {
      // Slow path (get primitive array and set individual elements).
      //
      // This is used if the input type is not an integer of the same size (note
      // signed/unsigned is ignored).
      jintArray jarr = env->NewIntArray(arr.size());
      if (!jarr) {
        return nullptr;
      }
      jint* elements =
          static_cast<jint*>(env->GetPrimitiveArrayCritical(jarr, nullptr));
      if (!elements) {
        return nullptr;
      }
      for (size_t i = 0; i < arr.size(); ++i) {
        elements[i] = static_cast<jint>(arr[i]);
      }
      env->ReleasePrimitiveArrayCritical(jarr, elements, 0);
      return jarr;
    }
  }
};

}  // namespace detail

/**
 * Convert a span to a jintArray.
 *
 * @param env JRE environment.
 * @param arr Span to convert.
 */
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, std::span<const T> arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

/**
 * Convert a span to a jintArray.
 *
 * @param env JRE environment.
 * @param arr Span to convert.
 */
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, std::span<T> arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

/**
 * Convert a SmallVector to a jintArray.
 *
 * This is required in addition to ArrayRef because template resolution occurs
 * prior to implicit conversions.
 *
 * @param env JRE environment.
 * @param arr SmallVector to convert.
 */
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, const SmallVectorImpl<T>& arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

/**
 * Convert a std::vector to a jintArray.
 *
 * This is required in addition to ArrayRef because template resolution occurs
 * prior to implicit conversions.
 *
 * @param env JRE environment.
 * @param arr SmallVector to convert.
 */
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, const std::vector<T>& arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

/**
 * Convert a span into a jbyteArray.
 *
 * @param env JRE environment.
 * @param str span to convert.
 */
inline jbyteArray MakeJByteArray(JNIEnv* env, std::span<const uint8_t> str) {
  jbyteArray jarr = env->NewByteArray(str.size());
  if (!jarr) {
    return nullptr;
  }
  env->SetByteArrayRegion(jarr, 0, str.size(),
                          reinterpret_cast<const jbyte*>(str.data()));
  return jarr;
}

/**
 * Convert an array of integers into a jbooleanArray.
 *
 * @param env JRE environment.
 * @param arr Array to convert.
 */
inline jbooleanArray MakeJBooleanArray(JNIEnv* env, std::span<const int> arr) {
  jbooleanArray jarr = env->NewBooleanArray(arr.size());
  if (!jarr) {
    return nullptr;
  }
  jboolean* elements =
      static_cast<jboolean*>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    elements[i] = arr[i] ? JNI_TRUE : JNI_FALSE;
  }
  env->ReleasePrimitiveArrayCritical(jarr, elements, 0);
  return jarr;
}

/**
 * Convert an array of booleans into a jbooleanArray.
 *
 * @param env JRE environment.
 * @param arr Array to convert.
 */
inline jbooleanArray MakeJBooleanArray(JNIEnv* env, std::span<const bool> arr) {
  jbooleanArray jarr = env->NewBooleanArray(arr.size());
  if (!jarr) {
    return nullptr;
  }
  jboolean* elements =
      static_cast<jboolean*>(env->GetPrimitiveArrayCritical(jarr, nullptr));
  if (!elements) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    elements[i] = arr[i] ? JNI_TRUE : JNI_FALSE;
  }
  env->ReleasePrimitiveArrayCritical(jarr, elements, 0);
  return jarr;
}

// Other MakeJ*Array conversions.

#define WPI_JNI_MAKEJARRAY(T, F)                                         \
  inline T##Array MakeJ##F##Array(JNIEnv* env, std::span<const T> arr) { \
    T##Array jarr = env->New##F##Array(arr.size());                      \
    if (!jarr) {                                                         \
      return nullptr;                                                    \
    }                                                                    \
    env->Set##F##ArrayRegion(jarr, 0, arr.size(), arr.data());           \
    return jarr;                                                         \
  }

WPI_JNI_MAKEJARRAY(jboolean, Boolean)
WPI_JNI_MAKEJARRAY(jbyte, Byte)
WPI_JNI_MAKEJARRAY(jshort, Short)
WPI_JNI_MAKEJARRAY(jfloat, Float)
WPI_JNI_MAKEJARRAY(jdouble, Double)

#undef WPI_JNI_MAKEJARRAY

template <class T>
  requires(sizeof(typename T::value_type) == sizeof(jlong) &&
           std::integral<typename T::value_type>)
inline jlongArray MakeJLongArray(JNIEnv* env, const T& arr) {
  jlongArray jarr = env->NewLongArray(arr.size());
  if (!jarr) {
    return nullptr;
  }
  env->SetLongArrayRegion(jarr, 0, arr.size(),
                          reinterpret_cast<const jlong*>(arr.data()));
  return jarr;
}

/**
 * Convert an array of std::string into a jarray of jstring.
 *
 * @param env JRE environment.
 * @param arr Array to convert.
 */
inline jobjectArray MakeJStringArray(JNIEnv* env,
                                     std::span<const std::string> arr) {
  static JClass stringCls{env, "java/lang/String"};
  if (!stringCls) {
    return nullptr;
  }
  jobjectArray jarr = env->NewObjectArray(arr.size(), stringCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jstring> elem{env, MakeJString(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

/**
 * Convert an array of std::string into a jarray of jstring.
 *
 * @param env JRE environment.
 * @param arr Array to convert.
 */
inline jobjectArray MakeJStringArray(JNIEnv* env,
                                     std::span<std::string_view> arr) {
  static JClass stringCls{env, "java/lang/String"};
  if (!stringCls) {
    return nullptr;
  }
  jobjectArray jarr = env->NewObjectArray(arr.size(), stringCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jstring> elem{env, MakeJString(env, arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

/**
 * Generic callback thread implementation.
 *
 * JNI's AttachCurrentThread() creates a Java Thread object on every
 * invocation, which is both time inefficient and causes issues with Eclipse
 * (which tries to keep a thread list up-to-date and thus gets swamped).
 *
 * Instead, this class attaches just once.  When a hardware notification
 * occurs, a condition variable wakes up this thread and this thread actually
 * makes the call into Java.
 *
 * The template parameter T is the message being passed to the callback, but
 * also needs to provide the following functions:
 *  static JavaVM* GetJVM();
 *  static const char* GetName();
 *  void CallJava(JNIEnv *env, jobject func, jmethodID mid);
 */
template <typename T>
class JCallbackThread : public SafeThread {
 public:
  void Main() override;

  std::queue<T> m_queue;
  jobject m_func = nullptr;
  jmethodID m_mid;
};

template <typename T>
class JCallbackManager : public SafeThreadOwner<JCallbackThread<T>> {
 public:
  JCallbackManager() { this->SetJoinAtExit(false); }
  void SetFunc(JNIEnv* env, jobject func, jmethodID mid);

  template <typename... Args>
  void Send(Args&&... args);
};

template <typename T>
void JCallbackManager<T>::SetFunc(JNIEnv* env, jobject func, jmethodID mid) {
  auto thr = this->GetThread();
  if (!thr) {
    return;
  }
  // free global reference
  if (thr->m_func) {
    env->DeleteGlobalRef(thr->m_func);
  }
  // create global reference
  thr->m_func = env->NewGlobalRef(func);
  thr->m_mid = mid;
}

template <typename T>
template <typename... Args>
void JCallbackManager<T>::Send(Args&&... args) {
  auto thr = this->GetThread();
  if (!thr) {
    return;
  }
  thr->m_queue.emplace(std::forward<Args>(args)...);
  thr->m_cond.notify_one();
}

template <typename T>
void JCallbackThread<T>::Main() {
  JNIEnv* env;
  JavaVMAttachArgs args;
  args.version = JNI_VERSION_1_2;
  args.name = const_cast<char*>(T::GetName());
  args.group = nullptr;
  jint rs = T::GetJVM()->AttachCurrentThreadAsDaemon(
      reinterpret_cast<void**>(&env), &args);
  if (rs != JNI_OK) {
    return;
  }

  std::unique_lock lock(m_mutex);
  while (m_active) {
    m_cond.wait(lock, [&] { return !(m_active && m_queue.empty()); });
    if (!m_active) {
      break;
    }
    while (!m_queue.empty()) {
      if (!m_active) {
        break;
      }
      auto item = std::move(m_queue.front());
      m_queue.pop();
      auto func = m_func;
      auto mid = m_mid;
      lock.unlock();  // don't hold mutex during callback execution
      item.CallJava(env, func, mid);
      if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
      }
      lock.lock();
    }
  }

  JavaVM* jvm = T::GetJVM();
  if (jvm) {
    jvm->DetachCurrentThread();
  }
}

template <typename T>
class JSingletonCallbackManager : public JCallbackManager<T> {
 public:
  static JSingletonCallbackManager<T>& GetInstance() {
    static JSingletonCallbackManager<T> instance;
    return instance;
  }
};

inline std::string GetJavaStackTrace(JNIEnv* env, std::string_view skipPrefix) {
  // create a throwable
  static JClass throwableCls(env, "java/lang/Throwable");
  if (!throwableCls) {
    return "";
  }
  static jmethodID constructorId = nullptr;
  if (!constructorId) {
    constructorId = env->GetMethodID(throwableCls, "<init>", "()V");
  }
  JLocal<jobject> throwable(env, env->NewObject(throwableCls, constructorId));

  // retrieve information from the exception.
  // get method id
  // getStackTrace returns an array of StackTraceElement
  static jmethodID getStackTraceId = nullptr;
  if (!getStackTraceId) {
    getStackTraceId = env->GetMethodID(throwableCls, "getStackTrace",
                                       "()[Ljava/lang/StackTraceElement;");
  }

  // call getStackTrace
  JLocal<jobjectArray> stackTrace(
      env, static_cast<jobjectArray>(
               env->CallObjectMethod(throwable, getStackTraceId)));

  if (!stackTrace) {
    return "";
  }

  // get length of the array
  jsize stackTraceLength = env->GetArrayLength(stackTrace);

  // get toString methodId of StackTraceElement class
  static JClass stackTraceElementCls(env, "java/lang/StackTraceElement");
  if (!stackTraceElementCls) {
    return "";
  }
  static jmethodID toStringId = nullptr;
  if (!toStringId) {
    toStringId = env->GetMethodID(stackTraceElementCls, "toString",
                                  "()Ljava/lang/String;");
  }

  bool foundFirst = false;
  std::string buf;
  raw_string_ostream oss(buf);
  for (jsize i = 0; i < stackTraceLength; i++) {
    // add the result of toString method of each element in the result
    JLocal<jobject> curStackTraceElement(
        env, env->GetObjectArrayElement(stackTrace, i));

    // call to string on the object
    JLocal<jstring> stackElementString(
        env, static_cast<jstring>(
                 env->CallObjectMethod(curStackTraceElement, toStringId)));

    if (!stackElementString) {
      return "";
    }

    // add a line to res
    JStringRef elem(env, stackElementString);
    if (!foundFirst) {
      if (wpi::starts_with(elem, skipPrefix)) {
        continue;
      }
      foundFirst = true;
    }
    oss << "\tat " << elem << '\n';
  }

  return oss.str();
}

inline std::string GetJavaStackTrace(JNIEnv* env, std::string* func,
                                     std::string_view excludeFuncPrefix) {
  // create a throwable
  static JClass throwableCls(env, "java/lang/Throwable");
  if (!throwableCls) {
    return "";
  }
  static jmethodID constructorId = nullptr;
  if (!constructorId) {
    constructorId = env->GetMethodID(throwableCls, "<init>", "()V");
  }
  JLocal<jobject> throwable(env, env->NewObject(throwableCls, constructorId));

  // retrieve information from the exception.
  // get method id
  // getStackTrace returns an array of StackTraceElement
  static jmethodID getStackTraceId = nullptr;
  if (!getStackTraceId) {
    getStackTraceId = env->GetMethodID(throwableCls, "getStackTrace",
                                       "()[Ljava/lang/StackTraceElement;");
  }

  // call getStackTrace
  JLocal<jobjectArray> stackTrace(
      env, static_cast<jobjectArray>(
               env->CallObjectMethod(throwable, getStackTraceId)));

  if (!stackTrace) {
    return "";
  }

  // get length of the array
  jsize stackTraceLength = env->GetArrayLength(stackTrace);

  // get toString methodId of StackTraceElement class
  static JClass stackTraceElementCls(env, "java/lang/StackTraceElement");
  if (!stackTraceElementCls) {
    return "";
  }
  static jmethodID toStringId = nullptr;
  if (!toStringId) {
    toStringId = env->GetMethodID(stackTraceElementCls, "toString",
                                  "()Ljava/lang/String;");
  }

  bool haveLoc = false;
  std::string buf;
  raw_string_ostream oss(buf);
  for (jsize i = 0; i < stackTraceLength; i++) {
    // add the result of toString method of each element in the result
    JLocal<jobject> curStackTraceElement(
        env, env->GetObjectArrayElement(stackTrace, i));

    // call to string on the object
    JLocal<jstring> stackElementString(
        env, static_cast<jstring>(
                 env->CallObjectMethod(curStackTraceElement, toStringId)));

    if (!stackElementString) {
      return "";
    }

    // add a line to res
    JStringRef elem(env, stackElementString);
    oss << elem << '\n';

    if (func) {
      // func is caller of immediate caller (if there was one)
      // or, if we see it, the first user function
      if (i == 1) {
        *func = elem.str();
      } else if (i > 1 && !haveLoc && !excludeFuncPrefix.empty() &&
                 !wpi::starts_with(elem, excludeFuncPrefix)) {
        *func = elem.str();
        haveLoc = true;
      }
    }
  }

  return oss.str();
}

/**
 * Finds an exception class and keep it as a global reference.
 *
 * Similar to JClass, but provides Throw methods. Use with caution, as the
 * destructor does NOT call DeleteGlobalRef due to potential shutdown issues
 * with doing so.
 */
class JException : public JClass {
 public:
  JException() = default;
  JException(JNIEnv* env, const char* name) : JClass(env, name) {
    if (m_cls) {
      m_constructor =
          env->GetMethodID(m_cls, "<init>", "(Ljava/lang/String;)V");
    }
  }

  void Throw(JNIEnv* env, jstring msg) {
    jobject exception = env->NewObject(m_cls, m_constructor, msg);
    env->Throw(static_cast<jthrowable>(exception));
  }

  void Throw(JNIEnv* env, std::string_view msg) {
    Throw(env, MakeJString(env, msg));
  }

  explicit operator bool() const { return m_constructor; }

 private:
  jmethodID m_constructor = nullptr;
};

struct JExceptionInit {
  const char* name;
  JException* cls;
};

}  // namespace wpi::java

#endif  // WPIUTIL_WPI_JNI_UTIL_H_
