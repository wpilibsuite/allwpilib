#pragma once

#include <memory>

#include "SourceImpl.h"

namespace cs {
class UsbCameraImpl : public SourceImpl, public std::enable_shared_from_this<UsbCameraImpl> {

};
}
