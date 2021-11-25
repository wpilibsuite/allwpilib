#include "wpi/mDNSResolver.h"

using namespace wpi;

struct mDNSResolver::Impl {};

mDNSResolver::mDNSResolver(std::string_view serviceType,
                           mDnsRevolveCompletionFunc onFound) {}

mDNSResolver::~mDNSResolver() noexcept {}

void mDNSResolver::Start() {}

void mDNSResolver::Stop() {}
