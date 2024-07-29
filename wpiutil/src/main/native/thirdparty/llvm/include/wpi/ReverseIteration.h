#ifndef WPIUTIL_WPI_REVERSEITERATION_H
#define WPIUTIL_WPI_REVERSEITERATION_H

#include "wpi/PointerLikeTypeTraits.h"

namespace wpi {

template<class T = void *>
bool shouldReverseIterate() {
#if LLVM_ENABLE_REVERSE_ITERATION
  return detail::IsPointerLike<T>::value;
#else
  return false;
#endif
}

} // namespace wpi
#endif
