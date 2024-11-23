#ifndef IXM_BREAKPOINT_HPP
#define IXM_BREAKPOINT_HPP

#include <debugging/detail/psnip_debug_trap.h>

namespace wpi {

bool is_debugger_present() noexcept;

#if defined(__GNUC__) && !defined(__clang__)
[[gnu::flatten]]
#endif
inline void breakpoint() noexcept
{
	psnip_trap();
}


#if defined(__GNUC__) && !defined(__clang__)
[[gnu::flatten]]
#endif
inline void breakpoint_if_debugging() noexcept
{
	if (is_debugger_present()) breakpoint();
}

} // namespace wpi

#endif
