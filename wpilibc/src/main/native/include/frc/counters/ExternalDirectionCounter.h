#pragma once

#include <memory>

#include <hal/Types.h>

#include "EdgeConfiguration.h"
#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {
class DigitalSource;

class ExternalDirectionCounter : public ErrorBase, public Sendable, public SendableHelper<ExternalDirectionCounter> {
  public:
    explicit ExternalDirectionCounter(DigitalSource& countSource, DigitalSource& directionSource);
    explicit ExternalDirectionCounter(DigitalSource* countSource, DigitalSource* directionSource);
    explicit ExternalDirectionCounter(std::shared_ptr<DigitalSource> countSource, std::shared_ptr<DigitalSource> directionSource);

    ~ExternalDirectionCounter() override;

    ExternalDirectionCounter(ExternalDirectionCounter&&) = default;
    ExternalDirectionCounter& operator=(ExternalDirectionCounter&&) = default;

    int GetCount() const;

    void SetReverseDirection(bool reverseDirection);
    void Reset();

    void SetEdgeConfiguration(EdgeConfiguration configuration);

 protected:
  void InitSendable(SendableBuilder& builder) override;

   private:
  void InitExternalDirectionCounter();
  std::shared_ptr<DigitalSource> m_countSource;
  std::shared_ptr<DigitalSource> m_directionSource;
  hal::Handle<HAL_CounterHandle> m_handle;
};
}
