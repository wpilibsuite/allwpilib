#ifdef _WIN32
#include <threads.h>
#endif

namespace wpi::hack {
void CallACThreadFunction() {
#ifdef _WIN32
  mtx_t Mtx{};
  mtx_init(&Mtx, mtx_plain);
#endif
}
}  // namespace wpi::hack
