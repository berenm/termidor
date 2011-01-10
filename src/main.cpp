/**
 * @file
 * @date Dec 28, 2010
 */

#define BOOST_CGI_NO_LOGGING
#include "boost/cgi/fcgi.hpp"
#include <iostream>

#include "anyterm/session_manager.hpp"

//
// This function writes the title and map contents to the ostream in an
// HTML-encoded format (to make them easier on the eye).
//
template< typename OStream, typename Request, typename Map >
void format_map(OStream& os, Request& req, Map& m, const std::string& title) {
  os << "<ul class=\"data-map\">"
    "<div class=\"title\">" << title << "</div>";

  if (m.empty()) {
    os << "<li>EMPTY</li>";
  } else {
    for (typename Map::const_iterator i = m.begin(); i != m.end(); ++i) {
      os << "<li>"
        "<div class=\"name\">" << i->first << "</div>"
        "<div class=\"value\">" << i->second << "</div>"
        "</li>";
    }
  }
  os << "<div class=\"clear\"></div></ul>";
}

int handle_request(::anyterm::session_manager& manager,
                   ::boost::fcgi::acceptor& a,
                   ::boost::fcgi::request& request_inout) {
  ::boost::system::error_code ec;

  // Load in the request data so we can access it easily.
  request_inout.load(::boost::fcgi::parse_all); // Read and parse STDIN (ie. POST) data.

  ::boost::shared_ptr< ::anyterm::session > session_ptr = manager.get_session(request_inout.session.id());

  ::std::string action = request_inout.post["action"];
  if (action.empty()) {
    action = request_inout.get["action"];
  }

  ::boost::fcgi::response response;
  if (action.compare("open") == 0) {
    ::std::uint16_t row_count = ::boost::lexical_cast< int >(request_inout.post["row_count"]);
    ::std::uint16_t column_count = ::boost::lexical_cast< int >(request_inout.post["column_count"]);
    ::std::string username = request_inout.post["username"];

    // Close last session and start a new one.
    request_inout.session.loaded(false);
    request_inout.session.id("");
    request_inout.start_session();

    manager.new_session(request_inout.session.id(), username, row_count, column_count);

  } else if (action.compare("resize") == 0 && session_ptr) {
    ::std::cout << request_inout.post["row_count"] << ::std::endl;
    ::std::cout << request_inout.post["column_count"] << ::std::endl;
    ::std::uint16_t row_count = ::boost::lexical_cast< int >(request_inout.post["row_count"]);
    ::std::uint16_t column_count = ::boost::lexical_cast< int >(request_inout.post["column_count"]);

    session_ptr->resize(row_count, column_count);

  } else if (action.compare("write") == 0 && session_ptr) {
    session_ptr->write(request_inout.post["data"]);

  } else if (action.compare("read") == 0 && session_ptr) {
    ::std::string data = session_ptr->read();

    if (data.empty()) {
      response << ::boost::fcgi::http::not_modified;
      response << ::boost::fcgi::header("Status", "304");
    } else {
      response << ::boost::fcgi::content_type("text/html");
      response << ::boost::fcgi::charset("utf-8");
      response << data;
    }
  }

  // Response headers can be added at any time before send/flushing it:
  response << ::boost::fcgi::content_length(response);

  int status = 0;
  try {
    status = ::boost::fcgi::commit(request_inout, response);
  } catch (::boost::system::system_error const& e) {
    ::std::clog << "Catched exception: " << e.what() << ::std::endl;
  }

  // The return value isn't used for anything.
  return status;
}

int main() {
  ::std::setlocale(LC_ALL, "utf8");

  g_type_init();

  ::anyterm::session_manager manager;
  //  Anyterm anyterm("", "", "ascii", true, 20);

  try {
    // Make a `service` (more about this in other examples).
    ::boost::fcgi::service s;

    // Make an `acceptor` for accepting requests through.
    ::boost::fcgi::acceptor a(s, 8088);

    do {
      try {
        a.accept(::boost::bind(&handle_request, ::boost::ref(manager), boost::ref(a), _1));
      } catch (boost::system::system_error const& se) {
        std::cerr << "[fcgi] System error: " << se.what() << std::endl;
      }
    } while (true);

    return 0;

  } catch (boost::system::system_error const& se) {
    // This is the type of exception thrown by the library.
    std::cerr << "[fcgi] System error: " << se.what() << std::endl;
    return -1;
    //  } catch (std::exception const& e) {
    //    // Catch any other exceptions
    //    std::cerr << "[fcgi] Exception: " << e.what() << std::endl;
    //    return -2;
    //  } catch (...) {
    //    std::cerr << "[fcgi] Uncaught exception!" << std::endl;
    //    return -3;
  }
}
