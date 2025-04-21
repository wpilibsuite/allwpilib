#if defined(linux) || defined(__linux) || defined(__linux__) \
	|| defined(__gnu_linux__)

#	include <debugging.hpp>

#	include <atomic>
#	include <fstream>
#	include <string>

namespace wpi {

bool is_debugger_present() noexcept
{
	std::ifstream proc_self_status_f("/proc/self/status");
	if (!proc_self_status_f) return false;
	std::string buffer(256, '\0');
	proc_self_status_f.read(&buffer[0], 256);
	auto index = buffer.find("TracerPid:\t");
	if (index == buffer.npos) return false;
	return buffer[index + 11] != '0';
}

} // namespace wpi

#endif
