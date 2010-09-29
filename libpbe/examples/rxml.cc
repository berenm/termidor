#include "rxml.hh"

#include <Exception.hh>

#include <string>
#include <iostream>

using namespace std;
using namespace pbe;


static void dump_attrs(rxml::attributes attrs)
{
  for (rxml::attribute_iterator i = attrs.begin();
       i != attrs.end(); ++i) {
    string n(i->name().begin(),i->name().end());
    string v(i->value().begin(),i->value().end());
    cout << n << "='" << v << "' ";
  }
}


int main(int argc, char* argv[])
{
  try { try {

    if (argc<2) {
      throw "Usage: rxml <file.xml>";
    }
    string fn(argv[1]);

    rxml::file doc(fn);

    cout << "xmldecl attributes: ";
    dump_attrs(doc.xmldecl_attrs());
    cout << "\n";

    rxml::element root = doc.root();

    string root_name(root.name().begin(),root.name().end());
    cout << "root element name = '" << root_name << "'\n";

    for (rxml::attribute_iterator i = root.attrs().begin();
         i != root.attrs().end(); ++i) {
      cout << "  attribute '" << i->name().str() << "' = '" << i->value().str() << "'\n";
    }

    for (rxml::child_element_iterator i = root.child_elements().begin();
         i != root.child_elements().end(); ++i) {
      cout << "  child element named '" << i->name().str() << "'\n";
      for (rxml::child_node_iterator j = i->child_nodes().begin();
           j != i->child_nodes().end(); ++j) {
        if (j->is_text_node()) {
          cout << "    text node '" << j->text().str() << "'\n";
        } else if (j->is_element()) {
          cout << "    element named '" << j->elem().name().str() << "'\n";
        }
      }
      cout << "  text = '" << i->text() << "'\n";
    }


    cout << "iterating through all descendants:\n";
    for (rxml::descendant_node_iterator i = root.descendant_nodes().begin();
         i != root.descendant_nodes().end(); ++i) {
        cout << "level=" << i.level() << ": ";
        if (i->is_text_node()) {
          cout << "text node '" << i->text().str() << "'\n";
        } else if (i->is_element()) {
          cout << "element named '" << i->elem().name().str() << "'\n";
        }
    }

    for (rxml::descendant_element_iterator i = root.descendant_elements().begin();
         i != root.descendant_elements().end(); ++i) {
      cout << string(i.level()*2,' ') << i->name().str() << "\n";
    }


    return 0;

  } RETHROW_MISC_EXCEPTIONS }
  catch (Exception& E) {
    E.report(cerr);
    return 1;
  }
}

