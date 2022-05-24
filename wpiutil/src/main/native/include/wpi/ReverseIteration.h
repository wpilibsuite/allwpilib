#ifndef WPI_REVERSEITERATION_H
#define WPI_REVERSEITERATION_H

#include "wpi/PointerLikeTypeTraits.h"

namespace wpi {

template<class T = void *>
constexpr bool shouldReverseIterate() {
  return false;
}

}

#endif
