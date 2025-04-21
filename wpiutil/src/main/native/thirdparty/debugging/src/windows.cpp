#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) \
	|| defined(__TOS_WIN__) || defined(__WINDOWS__)

#	include <debugging.hpp>

#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>

namespace wpi {

bool is_debugger_present() noexcept
{
	return ::IsDebuggerPresent();
}

} // namespace wpi

#endif
