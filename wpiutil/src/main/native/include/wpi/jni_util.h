// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_JNI_UTIL_H_
#define WPIUTIL_WPI_JNI_UTIL_H_

#include <jni.h>

#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/ArrayRef.h"
#include "wpi/ConvertUTF.h"
#include "wpi/SafeThread.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/deprecated.h"
#include "wpi/mutex.h"
#include "wpi/raw_ostream.h"

/** Java Native Interface (JNI) utility functions */
namespace wpi::java {

// Gets a Java stack trace.  Also provides the last function
// in the stack trace not starting with excludeFuncPrefix (useful for e.g.
// finding the first user call to a series of library functions).
std::string GetJavaStackTrace(JNIEnv* env, std::string* func = nullptr,
                              StringRef excludeFuncPrefix = StringRef());

// Finds a class and keep it as a global reference.
// Use with caution, as the destructor does NOT call DeleteGlobalRef due
// to potential shutdown issues with doing so.
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

// Container class for cleaning up Java local references.
// The destructor calls DeleteLocalRef.
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

// Java string (jstring) reference.  The string is provided as UTF8.
// This is not actually a reference, as it makes a copy of the string
// characters, but it's named this way for consistency.
class JStringRef {
 public:
  JStringRef(JNIEnv* env, jstring str) {
    if (str) {
      jsize size = env->GetStringLength(str);
      const jchar* chars = env->GetStringCritical(str, nullptr);
      if (chars) {
        convertUTF16ToUTF8String(makeArrayRef(chars, size), m_str);
        env->ReleaseStringCritical(str, chars);
      }
    } else {
      errs() << "JStringRef was passed a null pointer at \n"
             << GetJavaStackTrace(env);
    }
    // Ensure str is null-terminated.
    m_str.push_back('\0');
    m_str.pop_back();
  }

  operator StringRef() const { return m_str; }  // NOLINT
  StringRef str() const { return m_str; }
  const char* c_str() const { return m_str.data(); }
  size_t size() const { return m_str.size(); }

 private:
  SmallString<128> m_str;
};

// Details for J*ArrayRef and CriticalJ*ArrayRef
namespace detail {

template <typename C, typename T>
class JArrayRefInner {};

// Specialization of JArrayRefBase to provide StringRef conversion.
template <typename C>
class JArrayRefInner<C, jbyte> {
 public:
  operator StringRef() const { return str(); }

  StringRef str() const {
    auto arr = static_cast<const C*>(this)->array();
    if (arr.empty()) {
      return {};
    }
    return StringRef{reinterpret_cast<const char*>(arr.data()), arr.size()};
  }
};

// Base class for J*ArrayRef and CriticalJ*ArrayRef
template <typename T>
class JArrayRefBase : public JArrayRefInner<JArrayRefBase<T>, T> {
 public:
  explicit operator bool() const { return this->m_elements != nullptr; }

  operator ArrayRef<T>() const { return array(); }  // NOLINT

  ArrayRef<T> array() const {
    if (!this->m_elements) {
      return {};
    }
    return ArrayRef<T>{this->m_elements, this->m_size};
  }

  JArrayRefBase(const JArrayRefBase&) = delete;
  JArrayRefBase& operator=(const JArrayRefBase&) = delete;

  JArrayRefBase(JArrayRefBase&& oth)
      : m_env(oth.m_env),
        m_jarr(oth.m_jarr),
        m_size(oth.m_size),
        m_elements(oth.m_elements) {
    oth.m_jarr = nullptr;
    oth.m_elements = nullptr;
  }

  JArrayRefBase& operator=(JArrayRefBase&& oth) {
    this->m_env = oth.m_env;
    this->m_jarr = oth.m_jarr;
    this->m_size = oth.m_size;
    this->m_elements = oth.m_elements;
    oth.m_jarr = nullptr;
    oth.m_elements = nullptr;
    return *this;
  }

 protected:
  JArrayRefBase(JNIEnv* env, T* elements, size_t size) {
    this->m_env = env;
    this->m_jarr = nullptr;
    this->m_size = size;
    this->m_elements = elements;
  }

  JArrayRefBase(JNIEnv* env, jarray jarr, size_t size) {
    this->m_env = env;
    this->m_jarr = jarr;
    this->m_size = size;
    this->m_elements = nullptr;
  }

  JArrayRefBase(JNIEnv* env, jarray jarr)
      : JArrayRefBase(env, jarr, jarr ? env->GetArrayLength(jarr) : 0) {}

  JNIEnv* m_env;
  jarray m_jarr = nullptr;
  size_t m_size;
  T* m_elements;
};

}  // namespace detail

// Java array / DirectBuffer reference.

#define WPI_JNI_JARRAYREF(T, F)                                                \
  class J##F##ArrayRef : public detail::JArrayRefBase<T> {                     \
   public:                                                                     \
    J##F##ArrayRef(JNIEnv* env, jobject bb, int len)                           \
        : detail::JArrayRefBase<T>(                                            \
              env,                                                             \
              static_cast<T*>(bb ? env->GetDirectBufferAddress(bb) : nullptr), \
              len) {                                                           \
      if (!bb) {                                                               \
        errs() << "JArrayRef was passed a null pointer at \n"                  \
               << GetJavaStackTrace(env);                                      \
      }                                                                        \
    }                                                                          \
    J##F##ArrayRef(JNIEnv* env, T##Array jarr, int len)                        \
        : detail::JArrayRefBase<T>(env, jarr, len) {                           \
      if (jarr) {                                                              \
        m_elements = env->Get##F##ArrayElements(jarr, nullptr);                \
      } else {                                                                 \
        errs() << "JArrayRef was passed a null pointer at \n"                  \
               << GetJavaStackTrace(env);                                      \
      }                                                                        \
    }                                                                          \
    J##F##ArrayRef(JNIEnv* env, T##Array jarr)                                 \
        : detail::JArrayRefBase<T>(env, jarr) {                                \
      if (jarr) {                                                              \
        m_elements = env->Get##F##ArrayElements(jarr, nullptr);                \
      } else {                                                                 \
        errs() << "JArrayRef was passed a null pointer at \n"                  \
               << GetJavaStackTrace(env);                                      \
      }                                                                        \
    }                                                                          \
    ~J##F##ArrayRef() {                                                        \
      if (m_jarr && m_elements) {                                              \
        m_env->Release##F##ArrayElements(static_cast<T##Array>(m_jarr),        \
                                         m_elements, JNI_ABORT);               \
      }                                                                        \
    }                                                                          \
  };                                                                           \
                                                                               \
  class CriticalJ##F##ArrayRef : public detail::JArrayRefBase<T> {             \
   public:                                                                     \
    CriticalJ##F##ArrayRef(JNIEnv* env, T##Array jarr, int len)                \
        : detail::JArrayRefBase<T>(env, jarr, len) {                           \
      if (jarr) {                                                              \
        m_elements =                                                           \
            static_cast<T*>(env->GetPrimitiveArrayCritical(jarr, nullptr));    \
      } else {                                                                 \
        errs() << "JArrayRef was passed a null pointer at \n"                  \
               << GetJavaStackTrace(env);                                      \
      }                                                                        \
    }                                                                          \
    CriticalJ##F##ArrayRef(JNIEnv* env, T##Array jarr)                         \
        : detail::JArrayRefBase<T>(env, jarr) {                                \
      if (jarr) {                                                              \
        m_elements =                                                           \
            static_cast<T*>(env->GetPrimitiveArrayCritical(jarr, nullptr));    \
      } else {                                                                 \
        errs() << "JArrayRef was passed a null pointer at \n"                  \
               << GetJavaStackTrace(env);                                      \
      }                                                                        \
    }                                                                          \
    ~CriticalJ##F##ArrayRef() {                                                \
      if (m_jarr && m_elements) {                                              \
        m_env->ReleasePrimitiveArrayCritical(m_jarr, m_elements, JNI_ABORT);   \
      }                                                                        \
    }                                                                          \
  };

WPI_JNI_JARRAYREF(jboolean, Boolean)
WPI_JNI_JARRAYREF(jbyte, Byte)
WPI_JNI_JARRAYREF(jshort, Short)
WPI_JNI_JARRAYREF(jint, Int)
WPI_JNI_JARRAYREF(jlong, Long)
WPI_JNI_JARRAYREF(jfloat, Float)
WPI_JNI_JARRAYREF(jdouble, Double)

#undef WPI_JNI_JARRAYREF

//
// Conversions from C++ to Java objects
//

// Convert a UTF8 string into a jstring.
inline jstring MakeJString(JNIEnv* env, StringRef str) {
  SmallVector<UTF16, 128> chars;
  convertUTF8ToUTF16String(str, chars);
  return env->NewString(chars.begin(), chars.size());
}

// details for MakeJIntArray
namespace detail {

// Slow path (get primitive array and set individual elements).  This
// is used if the input type is not an integer of the same size (note
// signed/unsigned is ignored).
template <typename T,
          bool = (std::is_integral<T>::value && sizeof(jint) == sizeof(T))>
struct ConvertIntArray {
  static jintArray ToJava(JNIEnv* env, ArrayRef<T> arr) {
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
};

// Fast path (use SetIntArrayRegion)
template <typename T>
struct ConvertIntArray<T, true> {
  static jintArray ToJava(JNIEnv* env, ArrayRef<T> arr) {
    jintArray jarr = env->NewIntArray(arr.size());
    if (!jarr) {
      return nullptr;
    }
    env->SetIntArrayRegion(jarr, 0, arr.size(),
                           reinterpret_cast<const jint*>(arr.data()));
    return jarr;
  }
};

}  // namespace detail

// Convert an ArrayRef to a jintArray.
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, ArrayRef<T> arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

// Convert a SmallVector to a jintArray.  This is required in addition to
// ArrayRef because template resolution occurs prior to implicit conversions.
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, const SmallVectorImpl<T>& arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

// Convert a std::vector to a jintArray.  This is required in addition to
// ArrayRef because template resolution occurs prior to implicit conversions.
template <typename T>
inline jintArray MakeJIntArray(JNIEnv* env, const std::vector<T>& arr) {
  return detail::ConvertIntArray<T>::ToJava(env, arr);
}

// Convert a StringRef into a jbyteArray.
inline jbyteArray MakeJByteArray(JNIEnv* env, StringRef str) {
  jbyteArray jarr = env->NewByteArray(str.size());
  if (!jarr) {
    return nullptr;
  }
  env->SetByteArrayRegion(jarr, 0, str.size(),
                          reinterpret_cast<const jbyte*>(str.data()));
  return jarr;
}

// Convert an array of integers into a jbooleanArray.
inline jbooleanArray MakeJBooleanArray(JNIEnv* env, ArrayRef<int> arr) {
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

// Convert an array of booleans into a jbooleanArray.
inline jbooleanArray MakeJBooleanArray(JNIEnv* env, ArrayRef<bool> arr) {
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

#define WPI_JNI_MAKEJARRAY(T, F)                                  \
  inline T##Array MakeJ##F##Array(JNIEnv* env, ArrayRef<T> arr) { \
    T##Array jarr = env->New##F##Array(arr.size());               \
    if (!jarr) {                                                  \
      return nullptr;                                             \
    }                                                             \
    env->Set##F##ArrayRegion(jarr, 0, arr.size(), arr.data());    \
    return jarr;                                                  \
  }

WPI_JNI_MAKEJARRAY(jboolean, Boolean)
WPI_JNI_MAKEJARRAY(jbyte, Byte)
WPI_JNI_MAKEJARRAY(jshort, Short)
WPI_JNI_MAKEJARRAY(jlong, Long)
WPI_JNI_MAKEJARRAY(jfloat, Float)
WPI_JNI_MAKEJARRAY(jdouble, Double)

#undef WPI_JNI_MAKEJARRAY

// Convert an array of std::string into a jarray of jstring.
inline jobjectArray MakeJStringArray(JNIEnv* env, ArrayRef<std::string> arr) {
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

// Generic callback thread implementation.
//
// JNI's AttachCurrentThread() creates a Java Thread object on every
// invocation, which is both time inefficient and causes issues with Eclipse
// (which tries to keep a thread list up-to-date and thus gets swamped).
//
// Instead, this class attaches just once.  When a hardware notification
// occurs, a condition variable wakes up this thread and this thread actually
// makes the call into Java.
//
// The template parameter T is the message being passed to the callback, but
// also needs to provide the following functions:
//  static JavaVM* GetJVM();
//  static const char* GetName();
//  void CallJava(JNIEnv *env, jobject func, jmethodID mid);
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

inline std::string GetJavaStackTrace(JNIEnv* env, std::string* func,
                                     StringRef excludeFuncPrefix) {
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
                 !elem.str().startswith(excludeFuncPrefix)) {
        *func = elem.str();
        haveLoc = true;
      }
    }
  }

  return oss.str();
}

// Finds an exception class and keep it as a global reference.
// Similar to JClass, but provides Throw methods.
// Use with caution, as the destructor does NOT call DeleteGlobalRef due
// to potential shutdown issues with doing so.
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

  void Throw(JNIEnv* env, StringRef msg) { Throw(env, MakeJString(env, msg)); }

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
