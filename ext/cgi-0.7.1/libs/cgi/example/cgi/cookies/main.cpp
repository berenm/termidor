//                     -- main.hpp --
//
//         Copyright (c) Darren Garvey 2007-2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////
//
// Cookie Test With cTemplate
// --------------------------
//
// This file uses Google cTemplate [1] to show the benefits of using an HTML
// template engine [2]. Using cTemplate to separate how you show the response and
// how you figure out what to respond with, is keeping with the MVC paradigm.
//
// [1] - http://code.google.com/p/google-ctemplate/
// [2] - cTemplate isn't just an HTML templating engine.
//
//[headers
#include <boost/cgi/cgi.hpp>
#include <ctemplate/template.h>
#include <boost/throw_exception.hpp>
#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
//]

/**
 * The following example has a few stages.
 * It is best understood by compiling and testing it, and then looking at
 * the source code.
 */

//[main

namespace cgi = boost::cgi;
namespace fs = boost::filesystem;

// The types we use. Only here because this is an example.

// Uses cTemplate, from Google. It's simple and powerful.
typedef ctemplate::Template stencil_type;
// You will usually load a template and then populate variables in it
// using a TemplateDictionary.
typedef ctemplate::TemplateDictionary dictionary_type;

// This function just makes it easier to change the templating engine. It's
// only here to keep the cTemplate code out of the core of this example...
stencil_type* get_stencil(std::string const& filename)
{
  if (!fs::exists(filename))
    throw std::runtime_error(
      std::string("Template file not found: '")
        + fs::path(filename).string() + "'");
  else
    return ctemplate::Template::GetTemplate(
      filename, ctemplate::STRIP_WHITESPACE);
}

// Show the data in the passed map, updating the passed dictionary.
template<typename MapT, typename Dict>
void print_formatted_data(MapT& data, Dict& dict)
{
  Dict* subd = dict.AddSectionDictionary("DATA_MAP");
  if (data.empty())
    subd->ShowSection("EMPTY");
  else
    for(typename MapT::const_iterator iter=data.begin(), end = data.end(); iter != end; ++iter)
    {
      Dict* row_dict = subd->AddSectionDictionary("ROW");
      row_dict->SetValue("NAME", iter->first.c_str());
      row_dict->SetValue("VALUE", iter->second.c_str());
      row_dict->ShowSection("ROW");
    }
}


int main()
{
  try {
    cgi::request req;

    cgi::response resp;

    // Check if we are resetting the user.
    if (req.form.count("reset") && req.form["reset"] == "true")
    {
      resp<< cgi::cookie("name") // delete the 'name' cookie.
          << cgi::redirect(req, req.script_name()); // redirect them.
      return cgi::commit(req, resp);
    }

    if (req.form.count("name"))
    {
      // If requested by the user, delete the cookie.
      if (req.form.count("del"))
        resp<< cgi::cookie(req.form["name"]);
      else // Set the cookie.
        resp<< cgi::cookie(req.form["name"], req.form["value"]);
      resp<< cgi::redirect(req, req.script_name());
      // Exit here.
      return cgi::commit(req, resp);
    }

    dictionary_type dict("cookie-game dict");

    // First, see if they have a cookie set
    if (req.cookies.count("name"))
      dict.SetValueAndShowSection("USER_NAME", req.cookies["name"].c_str(),
        "HAS_NAME_IN_COOKIE_true");
    else
      dict.ShowSection("HAS_NAME_IN_COOKIE_false");

    print_formatted_data(req.cookies, dict);

    dict.SetValue("SCRIPT_NAME", req.script_name());
    // pick() looks up the key in the map, returns a default value
    // (ie. anonymous) if the key isn't found.
    dict.SetValue("COOKIE_NAME", req.form["name"]);
    dict.SetValue("COOKIE_VALUE", req.form["value"]);

    // Load the HTML stencil now from the index.html file.
    stencil_type* stencil = get_stencil("../stencils/index.html");

    // Expand the stencil with the the given dictionary into `output`.
    std::string output;
    stencil->Expand(&output, &dict);

    // Add the template to the response.
    resp<< cgi::content_type("text/html")
        << output;

    // Send the response to the requestor and return control.
    return cgi::commit(req, resp);

  }catch(boost::system::system_error& err){
    std::cerr<< "System Error: [" << err.code() << "] - " << err.what() << std::endl;
  }catch(std::exception const& e){
    std::cerr<< "Exception: [" << typeid(e).name() << "] - " << e.what() << std::endl;
  }catch(...){
    std::cerr<< "boom<blink>!</blink>";
  }
}
//]

