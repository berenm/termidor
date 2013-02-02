#include <locale>

#include "anyterm/http_server.hpp"

namespace anyterm {

  extern "C" int main(int const arg, char const* argv[]) {
    std::setlocale(LC_ALL, "utf8");

    anyterm::server server("127.0.0.1", "8088");

    server.run();
  }

}
