#include <cstring>
#include <string_view>
#include <fmt/format.h>

void show(std::string_view message_type, bool raw) {

}

void list() {

}

int main(int argc, char *argv[]) {
  if (argc > 1) { // 
    std::string_view command{argv[1]};
    std::string_view message_type{};

    if (command.compare("json")) { // 
      
    } else if (command.compare("csv")) {

    } else if (command.compare("extract")) {
      
    }
  } else {
    // no args given
    // report error
  }
}