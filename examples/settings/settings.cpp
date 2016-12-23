#include <chrono>
#include <thread>

#include "cscore.h"

#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    llvm::errs() << "Usage: settings camera [prop val] ... -- [prop val]...\n";
    llvm::errs() << "  Example: settings 1 brightness 30 raw_contrast 10\n";
    return 1;
  }

  int id;
  if (llvm::StringRef{argv[1]}.getAsInteger(10, id)) {
    llvm::errs() << "Expected number for camera\n";
    return 2;
  }

  cs::UsbCamera camera{"usbcam", id};

  // Set prior to connect
  int arg = 2;
  llvm::StringRef propName;
  for (; arg < argc && llvm::StringRef{argv[arg]} != "--"; ++arg) {
    if (propName.empty())
      propName = argv[arg];
    else {
      llvm::StringRef propVal{argv[arg]};
      int intVal;
      if (propVal.getAsInteger(10, intVal))
        camera.GetProperty(propName).SetString(propVal);
      else
        camera.GetProperty(propName).Set(intVal);
      propName = llvm::StringRef{};
    }
  }
  if (arg < argc && llvm::StringRef{argv[arg]} == "--") ++arg;

  // Wait to connect
  while (!camera.IsConnected())
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Set rest
  propName = llvm::StringRef{};
  for (; arg < argc; ++arg) {
    if (propName.empty())
      propName = argv[arg];
    else {
      llvm::StringRef propVal{argv[arg]};
      int intVal;
      if (propVal.getAsInteger(10, intVal))
        camera.GetProperty(propName).SetString(propVal);
      else
        camera.GetProperty(propName).Set(intVal);
      propName = llvm::StringRef{};
    }
  }

  // Print settings
  llvm::SmallString<64> buf;
  llvm::outs() << "Properties:\n";
  for (const auto& prop : camera.EnumerateProperties()) {
    llvm::outs() << "  " << prop.GetName();
    switch (prop.GetKind()) {
      case cs::VideoProperty::kBoolean:
        llvm::outs() << " (bool): " << "value=" << prop.Get()
                     << " default=" << prop.GetDefault();
        break;
      case cs::VideoProperty::kInteger:
        llvm::outs() << " (int): "
                     << "value=" << prop.Get() << " min=" << prop.GetMin()
                     << " max=" << prop.GetMax() << " step=" << prop.GetStep()
                     << " default=" << prop.GetDefault();
        break;
      case cs::VideoProperty::kString:
        llvm::outs() << " (string): " << prop.GetString(buf);
        break;
      case cs::VideoProperty::kEnum: {
        llvm::outs() << " (enum): " << "value=" << prop.Get();
        auto choices = prop.GetChoices();
        for (size_t i = 0; i < choices.size(); ++i) {
          if (choices[i].empty()) continue;
          llvm::outs() << "\n    " << i << ": " << choices[i];
        }
        break;
      }
      default:
        break;
    }
    llvm::outs() << '\n';
  }
}
