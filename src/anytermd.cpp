#include <boost/network/protocol/http/server.hpp>
#include <boost/network/uri.hpp>
#include <boost/network/uri/uri_io.hpp>

#include <iostream>
#include <unordered_map>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "anyterm/session_manager.hpp"

namespace anyterm {
  namespace http = boost::network::http;
  namespace uri  = boost::network::uri;

  struct handler;
  typedef http::server< handler > server;

  struct handler {
    anyterm::session_manager session_manager;

    struct parse {
      typedef std::unordered_map< std::string, std::vector< std::string > > string_map;

      static string_map parse_headers(server::request const& request) {
        string_map headers;
        std::for_each(request.headers.begin(), request.headers.end(), [&](http::request_header_narrow const& h) { headers.emplace(h.name, std::vector< std::string > { h.value }); });

        return headers;
      }

      static string_map parse_cookies(string_map const& headers) {
        auto const& it = headers.find("Cookie");
        string_map  cookies;

        if (it == headers.end())
          return cookies;

        std::vector< std::string > cookie_headers;
        for (auto const& cookie_header : it->second) {
          boost::split(cookie_headers, cookie_header, boost::is_any_of("; "), boost::token_compress_on);
        }

        for (auto const& cookie_header : cookie_headers) {
          std::vector< std::string > cookie_values;
          boost::split(cookie_values, cookie_header, boost::is_any_of("="), boost::token_compress_on);

          if (cookie_values.size() < 2)
            cookie_values.resize(2);

          std::string const cookie_key = cookie_values.front();
          cookie_values.erase(cookie_values.begin(), cookie_values.begin() + 1);

          auto const& cookie_it = cookies.find(cookie_key);
          if (cookie_it == cookies.end())
            cookies.emplace(cookie_key, cookie_values);
          else
            cookie_it->second.insert(cookie_it->second.end(), cookie_values.begin(), cookie_values.end());
        }

        return cookies;
      }

      static string_map parse_parameters(server::request const& request) {
        string_map     parameters;
        uri::uri const url("http://localhost" + request.destination);
        std::string    query = uri::query(url);

        if (request.method == "POST")
          query += "&" + request.body;

        std::vector< std::string > query_pairs;
        boost::split(query_pairs, query, boost::is_any_of("&"), boost::token_compress_on);

        for (auto const& query_pair : query_pairs) {
          std::vector< std::string > query_values;
          boost::split(query_values, query_pair, boost::is_any_of("="), boost::token_compress_on);

          if (query_values.size() < 2)
            query_values.resize(2);

          std::string const query_key = query_values.front();
          query_values.erase(query_values.begin(), query_values.begin() + 1);

          auto const& query_it = parameters.find(query_key);
          if (query_it == parameters.end())
            parameters.emplace(query_key, query_values);
          else
            query_it->second.insert(query_it->second.end(), query_values.begin(), query_values.end());
        }

        return parameters;
      }

      static std::string parse_session_id(string_map const& cookies) {
        auto const& it = cookies.find("_ssid");

        if (it == cookies.end())
          return "";

        return it->second.front();
      }

      parse(server::request const& request) :
        headers(parse_headers(request)),
        cookies(parse_cookies(this->headers)),
        parameters(parse_parameters(request)),
        session_id(parse_session_id(this->cookies))
      {}

      string_map const  headers;
      string_map const  cookies;
      string_map const  parameters;
      std::string const session_id;
    };

    void operator()(server::request const& request, server::response& response) {
      response = server::response::stock_reply(server::response::not_found, "");

      auto const& parsed = handler::parse(request);
      if (parsed.parameters.find("action") == parsed.parameters.end())
        { this->log("No action"); return; }

      std::string const& action       = parsed.parameters.at("action").front();
      std::string const& username     = parsed.parameters.find("username") != parsed.parameters.end() ? parsed.parameters.at("username").front() : "";
      std::string const& row_count    = parsed.parameters.find("row_count") != parsed.parameters.end() ? parsed.parameters.at("row_count").front() : "50";
      std::string const& column_count = parsed.parameters.find("column_count") != parsed.parameters.end() ? parsed.parameters.at("column_count").front() : "80";
      std::string const& write_data   = parsed.parameters.find("data") != parsed.parameters.end() ? parsed.parameters.at("data").front() : "";

      if (action == "open") {
        std::string const& session_id = this->session_manager.new_session(username, boost::lexical_cast< std::uint16_t >(row_count), boost::lexical_cast< std::uint16_t >(column_count));
        response = server::response::stock_reply(server::response::ok, "");
        response.headers.push_back({ "Set-Cookie", "_ssid=" + session_id });

        return;
      }

      if (parsed.session_id.empty())
        { this->log("No session id"); return; }

      anyterm::session_ptr const& session = this->session_manager.get_session(parsed.session_id);
      if (action == "resize")
        session->resize(boost::lexical_cast< std::uint16_t >(row_count), boost::lexical_cast< std::uint16_t >(column_count));
      else if (action == "write")
        session->write(uri::decoded(boost::replace_all_copy(write_data, "+", " ")));

      std::string const& data = session->read();
      if (data.empty()) {
        response = server::response::stock_reply(server::response::not_modified, "");
        response.headers.push_back({ "Status", "304" });
      } else {
        response = server::response::stock_reply(server::response::ok, data);
        response.headers.push_back({ "Content-Type", "text/html; charset=utf-8" });
      }
    }

    void log(server::string_type const& info) {
      std::cerr << "ERROR: " << info << '\n';
    }

  };

  extern "C" int main(int const arg, char const* argv[]) {
    std::setlocale(LC_ALL, "utf8");

    anyterm::handler handler;
    anyterm::server  server("0.0.0.0", "8088", handler);

    server.run();
  }

}
