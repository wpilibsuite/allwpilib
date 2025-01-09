#pragma once

#include <jni.h>
#include <string_view>

namespace wpi {

void ThrowIllegalArgumentException(JNIEnv* env, std::string_view msg);
void ThrowIndexOobException(JNIEnv* env, std::string_view msg);
void ThrowIOException(JNIEnv* env, std::string_view msg);
void ThrowNullPointerException(JNIEnv* env, std::string_view msg);

}  // namespace wpi