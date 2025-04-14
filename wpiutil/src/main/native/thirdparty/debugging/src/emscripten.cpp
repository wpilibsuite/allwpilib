#if defined(__EMSCRIPTEN__)

#	include <debugging.hpp>

#	include <atomic>
#	include <fstream>
#	include <string>

namespace wpi {

bool is_debugger_present() noexcept
{
  return false;
}

} // namespace wpi

#endif
