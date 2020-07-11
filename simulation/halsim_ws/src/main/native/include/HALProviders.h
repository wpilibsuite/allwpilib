#pragma once

#include <string>
#include <functional>

#include <hal/simulation/NotifyListener.h>

#include <wpi/mutex.h>
#include <wpi/json.h>

#include "BaseProvider.h"

typedef void (*HALCbRegisterIndexedFunc)(int32_t index, 
                                         HAL_NotifyCallback callback, 
                                         void* param, 
                                         HAL_Bool initialNotify);

typedef void (*HALCbRegisterSingleFunc)(HAL_NotifyCallback callback, 
                                        void* param, 
                                        HAL_Bool initialNotify);

class HALProvider : public BaseProvider {
  public:
    using BaseProvider::BaseProvider;

    void OnNetworkConnected(std::shared_ptr<HALSimWebsocketConnection> ws);

    static void OnStaticSimCallback(const char* name, void* param, const struct HAL_Value* value);

    void OnSimCallback(const char* name);

    virtual wpi::json OnSimValueChanged(const char* name) = 0;

  protected:
    wpi::mutex m_mutex;
};

class HALChannelProvider : public HALProvider {
  public:
    explicit HALChannelProvider(int32_t channel, const std::string& key);

  protected:
    int32_t m_channel;
};

using WSRegisterFunc = std::function<void(const std::string&, std::shared_ptr<BaseProvider>)>;

template <typename T>
void CreateProviders(const std::string& prefix, int numChannels, HALCbRegisterIndexedFunc halRegisterFunc, WSRegisterFunc wsRegisterFunc) {
  for (int32_t i = 0; i < numChannels; i++) {
    auto key = (prefix + "/" + wpi::Twine(i)).str();
    auto ptr = std::make_unique<T>(i, key);
    halRegisterFunc(i, HALProvider::OnStaticSimCallback, ptr.get(), true);
    wsRegisterFunc(key, std::move(ptr));
  }
}

template <typename T>
void CreateSingleProvider(const std::string* key, HALCbRegisterSingleFunc halRegisterFunc, WSRegisterFunc wsRegisterFunc) {
  auto ptr = std::make_unique<T>(key);
  halRegisterFunc(HALProvider::OnStaticSimCallback, ptr.get(), true);
  wsRegisterFunc(key, std::move(ptr));
}