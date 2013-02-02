#ifndef __ANYTERM_HTTP_SERVER_HPP__
#define __ANYTERM_HTTP_SERVER_HPP__

#include <string>

#include "anyterm/session_manager.hpp"

namespace anyterm {

  struct server {
    public:
      server(std::string const& address, std::string const& port);
      void run();

    private:
      anyterm::session_manager session_manager;
      std::string const        address;
      std::string const        port;
  };

}

#endif // ifndef __ANYTERM_HTTP_SERVER_HPP__
