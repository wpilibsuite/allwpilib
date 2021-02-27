#pragma once

namespace frc {
enum class EdgeConfiguration {
    kNone = 0,
    kRisingEdge = 0x1,
    kFallingEdge = 0x2,
    kBoth = 0x3
};
}
