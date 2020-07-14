#include <iostream>

#include <hal/Main.h>

#include "HALSimWS.h"
#include "ProviderContainer.h"
#include "DIOProvider.h"
#include "AnalogProvider.h"

#include <WSProviderContainer.h>
#include <WSProvider_Analog.h>

static ProviderContainer providers;

static wpilibws::ProviderContainer providers2;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif

int HALSIM_InitExtension(void) {
  std::cout << "WS Extention Initializing" << std::endl;

  auto hws = std::make_shared<HALSimWS>(providers);
  HALSimWS::SetInstance(hws);

  if (!hws->Initialize()) {
    return -1;
  }

  WSRegisterFunc registerFunc = std::bind(&ProviderContainer::Add, &providers, std::placeholders::_1, std::placeholders::_2);
  wpilibws::WSRegisterFunc regFunc = std::bind(&wpilibws::ProviderContainer::Add, &providers2, std::placeholders::_1, std::placeholders::_2);
  
  DIOProvider::Initialize(registerFunc);
  AnalogInProvider::Initialize(registerFunc);
  AnalogOutProvider::Initialize(registerFunc);

  wpilibws::HALSimWSProviderAnalogIn::Initialize(regFunc);

  HAL_SetMain(nullptr, HALSimWS::Main, HALSimWS::Exit);

  std::cout << "WS Extension initialized" << std::endl;
  return 0;
}

}
