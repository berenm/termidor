/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#include "anyterm/session.hpp"

namespace anyterm {

  session::session(std::string const& username, std::uint16_t const row_count, std::uint16_t const column_count, std::uint16_t const scrollback_count, std::chrono::seconds const timeout) :
    username(username), timeout(timeout), terminal() {
    this->terminal.set_size(row_count, column_count);
    this->terminal.set_scrollback_size(scrollback_count);
    this->touch();
  }

  session::~session() {}

  void session::resize(std::uint16_t const row_count, std::uint16_t const column_count) {
    this->terminal.set_size(row_count, column_count);
  }

  void session::write(std::string const& data) {
    if (!this->terminal.is_alive())
      this->terminal.login(username);

    if (!data.empty())
      this->terminal.write(data);

    this->touch();
  }

  std::string session::read() {
    if (!this->terminal.is_alive())
      this->terminal.login(this->username);

    // if (this->terminal.is_dirty())
      return this->terminal.read().to_html();

    return "";
  }

  void session::touch() {
    this->last_access = anyterm::clock::now();
  }

  bool session::timed_out() {
    return anyterm::clock::now() - this->last_access > timeout;
  }

} // namespace anyterm
