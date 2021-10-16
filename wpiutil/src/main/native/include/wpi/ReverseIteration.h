#ifndef WPIUTIL_WPI_REVERSEITERATION_H
#define WPIUTIL_WPI_REVERSEITERATION_H

#include "wpi/PointerLikeTypeTraits.h"

namespace wpi {

template<class T = void *>
bool shouldReverseIterate() {
#if 1
  return detail::IsPointerLike<T>::value;
#else
  return false;
#endif
}

}
#endif
