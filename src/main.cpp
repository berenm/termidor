/**
 * @file
 * @date Dec 28, 2010
 */

#define BOOST_CGI_NO_LOGGING
#include "boost/cgi/fcgi.hpp"
#include <iostream>

#include "anyterm/session_manager.hpp"

//#include "Anyterm.hh"

// The styling information for the page, just to make things look nicer.
static const char* gCSS_text = "body { padding: 0; margin: 3%; border-color: #efe; }"
  "ul.data-map .title"
  "{ font-weight: bold; font-size: large; }"
  "ul.data-map"
  "{ border: 1px dotted; padding: 2px 3px 2px 3px; margin-bottom: 3%; }"
  "ul.data-map li"
  "{ border-top: 1px dotted; overflow: auto; padding: 0; margin: 0; }"
  "ul.data-map div.name"
  "{ position: relative; float: left; width: 30%; font-weight: bold; }"
  "ul.data-map div.value"
  "{ position: relative; float: left; width: 65%; left: 1%;"
  " border-left: 1px solid; padding: 0 5px 0 5px;"
  " overflow: auto; white-space: pre; }"
  ".clear"
  "{ clear: both; }";

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

  // Construct a `response` object (makes writing/sending responses easier).
  ::boost::fcgi::response response;

  ::boost::shared_ptr< ::anyterm::session > session_ptr = manager.get_session(request_inout.session.id());

  ::std::string mode = request_inout.get["mode"];
  ::std::string action = request_inout.get["a"];
  ::std::string data;

  if (action.compare("open") == 0 || !session_ptr) {
    //    // Close last session and start a new one.
    request_inout.session.loaded(false);
    request_inout.session.id("");
    request_inout.start_session();
    manager.new_session(request_inout.session.id());
  } else if (action.compare("send") == 0 && session_ptr) {
    session_ptr->send(request_inout.get["k"]);
  } else if (action.compare("recv") == 0 && session_ptr) {
    //    do {
    data = session_ptr->receive();
    //      if(data.empty()) {
    //        ::boost::this_thread::sleep(::boost::posix_time::milliseconds(500));
    //      }
    //    } while (data.empty());
  }

  // Responses in CGI programs require at least a 'Content-type' header. The
  // library provides helpers for several common headers:


  //  format_map(::std::clog, request_inout, request_inout.get, "GET Variables");
  //  format_map(::std::clog, request_inout, request_inout.post, "POST Variables");
  if (mode.compare("json") == 0) {
    response << ::boost::fcgi::content_type("application/json");
    response << "{\"data\": \"" << data << "\"}";
  } else {
    response << ::boost::fcgi::content_type("text/html");
    response << "<html><head>"
      "<title>FastCGI Echo Example</title>"
      "<head><body>";

    response << "<pre>";
    response << data;
    response << "</pre>";
  }

  // Response headers can be added at any time before send/flushing it:
  response << ::boost::fcgi::content_length(response); // the content-length (returns std::size_t)

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
  g_type_init();

  ::anyterm::session_manager manager;
  //  Anyterm anyterm("", "", "ascii", true, 20);

  try {
    ::std::cerr << "*** Ping! ***" << '\n';
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
  } catch (std::exception const& e) {
    // Catch any other exceptions
    std::cerr << "[fcgi] Exception: " << e.what() << std::endl;
    return -2;
  } catch (...) {
    std::cerr << "[fcgi] Uncaught exception!" << std::endl;
    return -3;
  }
}
