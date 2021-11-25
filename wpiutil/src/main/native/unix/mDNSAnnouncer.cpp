#include "wpi/mDNSAnnouncer.h"

using namespace wpi;

struct mDNSAnnouncer::Impl {
};

mDNSAnnouncer::mDNSAnnouncer(std::string_view serviceName,
                             std::string_view serviceType,
                             std::string_view machineName,
                             wpi::span<std::pair<std::string, std::string>> txt) {
}

mDNSAnnouncer::~mDNSAnnouncer() noexcept {}

void mDNSAnnouncer::Start() {}

void mDNSAnnouncer::Stop() {}
