// include/rxml.hh
// This file is part of libpbe; see http://svn.chezphil.org/libpbe/
// (C) 2010 Philip Endecott

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef libpbe_rxml_hh
#define libpbe_rxml_hh

// rxml - Read XML
// ---------------
//
// This is a simple, low-memory, and hopefully fast XML reader.
// 
// rxml::document accesses an XML file in memory.  rxml::file is a 
// subclass that accesses a file by mapping it into memory.  The 
// advantage of memory-mapping is that parts of the file that are not 
// used need never be read, and those parts that are read can be 
// discarded by the operating system if it is short of memory.
// 
// The content of the document is then accessed by means of classes 
// with self-explanatory names i.e. element, attribute etc.  In each 
// case the class stores only a pointer into the memory-mapped region:
// there is no tree of objects created by parsing the XML source.
// Accessing, for example, the child nodes of an element is done by 
// parsing the XML source at that time.
//
// Because these objects are small, they should be passed by value.
// Because they store pointers into the document, the document must 
// exist for the lifespan of the other objects; they are invalidated 
// when it is destroyed.
//
// Iteration through elements and attributes is done in C++ style using 
// iterators.
//
// Simple example:
//
// <foo a="b"><x/><y/></foo>
//
// rxml::file doc("/path/to/file.xml");
// rxml::element root_element = doc.root();
// rxml::child_element_iterator i = root_element.child_elements().begin();
// while (i != root_element.child_elements().end()) {
//   rxml::element c = *i;
//   string name(c.name().begin(),c.name().end());
//   cout << "Child node is named " << name << "\n";
// }
//
// The basic iterator is the decendant_node_iterator.  This iterates 
// depth-first through all descendant nodes, and has a level() method 
// that indicates the depth below the ancestor element.  The other 
// iterators are implemented on top of descendant_node_iterator by 
// filtering:
// - descendant_element_iterator returns only nodes that are elements.
// - child_node_iterator returns only nodes with level()==1.
// - child_element_iterator returns only nodes that are elements with level()==1.
// - *_element_named_iterator filter on the name of the element.
//
// Note the following fundamental consequence of this design: iterating 
// through the childeren of a node takes time proportional to the total 
// number of descendants of that node, not just the number of child nodes. 
// Therefore a naive recursive traversal of a document can be O(N^2) for 
// an unbalanced structure or O(N log N) for a balanced structure, rather 
// than the O(N) that would be possible with other implementations. 
// This is a trade-off against the O(1) space requirements of this design. 
// O(N) traversal is possible by using a descendant_*_iterator rather than 
// a recursive series of child_*_iterators.
//
// Nodes can be identified by means of a set of is_*() predicates.  The 
// corresponding text_node or element can then be accessed.
//
// Names of elements, names and values of attributes, and content of 
// text nodes are accessible efficiently via begin-end iterator pairs.
//
// The iterators are all forward iterators.  In principle they could be 
// made bidirectional iterators by duplicating a lot of code.  But doing 
// anything other than a single-pass on the document might indicate an 
// un-optimal application for this library.
//
// element has a convenience member must_attribute(name) that returns an 
// attribute with the specified name, or throws notfound if none is present.
// Similarly must_child_element() returns a child attribute with the 
// specified name, or throws.
//
// The following are available as conveniences:
// - Element names, attribute names and values and the contents of text 
//   nodes can be accessed as std::strings using a str() method.
// - These can also be compared with std::strings and const char*s.
// - The concatentation of an element's descendant text nodes can be 
//   accessed via its text() method.
//
// The validity or well-formedness of the document is not checked. 
// "Garbage in, garbage out".  To guard against crashes due to bad or 
// malicious input, only the following garauntees are offered.  These 
// hold only if the document is terminated with a 0 byte (which is not 
// allowed to be part of an XML document).  This 0 byte can be added 
// after the end of the document, or the terminating newline or > can 
// be replaced by a 0.  rxml::file does the latter.
// - begin()-end() ranges will be iterable without error, i.e. they 
//   will not run off the end, though e.g. a non-terminated attribute 
//   value will extend to the end of the file.
// - errr....

// Implementation details
// ----------------------
// 
// Iterators are all implemented using boost::iterator_facade.
//
// Apart from the filtering iterators (child_*_iterator and *_element_iterator) 
// each class inherits from class base, which stores the pointer into the 
// document p.  p==NULL indicates an at-the-end condition. Each class also 
// has a default ctor that makes p NULL; this is used privately to return 
// end() iterators, but is public so that the objects can be default-
// constructable.   They also have a ctor that takes a const char* and 
// assign this to p; this is private and is used to return begin() iterators.


// Not yet implemented:
// - Character sets.
// - Ranges.

// Not implemented; not planned:
// - Processing instructions.  As far as I am aware, the only use of these is to 
//   associate XSL stylesheets with XML documents.
// - Retreiving comments.  Comments are skipped over while parsing.
// - Doctypes.  It attempts to skip the doctype while parsing, but this will 
//   fail if the doctype contains a >; this is allowed in the SystemLiteral, 
//   PubidLiteral and intSubset.  The doctypes typically found in e.g. XHTML 
//   etc. will be OK.
// - UTF-16, or other character sets that are not ASCII supersets.  The best way 
//   to handle these would be to convert to UTF-8 first.
// - Well-formedness checking.


#include <sentinel_iterator.hh>
#include <succpred.hh>
#include <predicates.hh>
#include <equal_ranges.hh>
#include <safe_bool.hh>
#include <starts_ends_with.hh>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>

#include <cctype>
#include <string>
#include <vector>

namespace pbe {
namespace rxml {


typedef bool(*char_pred_t)(char);

template <char_pred_t pred>
struct mk_pred_functor {
  bool operator()(char c) const {
    return pred(c);
  }
};


struct base;
class attribute_name;
class attribute_value;
class attribute;
class attribute_iterator;
class attributes;
class text_node;
class node;
class descendant_node_iterator;
class descendant_nodes_t;
class descendant_elements_iterator;
class descendant_elements_t;
class child_node_iterator;
class child_nodes_t;
class child_element_iterator;
class child_elements_t;
template <typename NAME_T> class descendant_element_named_iterator;
template <typename NAME_T> class descendant_elements_named_t;
template <typename NAME_T> class child_element_named_iterator;
template <typename NAME_T> class child_elements_named_t;
class element_name;
class element;
class document;
struct file_base_impl;
struct file_base;
class file;

}
}

namespace boost {
  template < >
  struct is_POD< pbe::rxml::element > : public true_type {};
}

namespace pbe {
namespace rxml {

struct UnexpectedEndOfDocument {}; // Exception thrown when the document's terminating 
                                   // NULL byte is encountered.

struct UnknownEntityReference {};  // Exception thrown when an unknown entity reference 
                                   // (e.g. &foo;) is encountered during decoding.

extern char decode_ref(const char* p);


struct base {
  const char* p;
  base(): p(NULL) {}
  base(const char* p_): p(p_) {}
};


// This CRTP base class is used to add common convenience features all all 
// "string-like" classes.
template <typename DERIVED>
class stringy {
  DERIVED& derived() { return static_cast<DERIVED&>(*this); }
  const DERIVED& derived() const { return static_cast<const DERIVED&>(*this); }
public:
  std::string str() const { return std::string(derived().begin(),derived().end()); }
  bool operator==(const char* s) const {
    return pbe::equal_ranges(derived().begin(),derived().end(),const_cstr_iterator(s),const_cstr_iterator());
  }
  bool operator==(std::string s) const {
    return pbe::equal_ranges(derived().begin(),derived().end(),s.begin(),s.end());
  }
  bool operator!=(const char* s) const { return !operator==(s); }
  bool operator!=(std::string s) const { return !operator==(s); }
};


// This CRTP base class is used to add common convenience features all all 
// "container-like" classes.
template <typename DERIVED>
class containery {
  DERIVED& derived() { return static_cast<DERIVED&>(*this); }
  const DERIVED& derived() const { return static_cast<const DERIVED&>(*this); }
public:
  bool empty() const { return derived().begin() == derived().end(); }
};


class attribute_name:
  base,
  public stringy<attribute_name> {
  // p points to the first character of the attribute's name.
  explicit attribute_name(const char* p): base(p) {}
  friend class attribute;
public:
  attribute_name() {}
  typedef const_predicate_iterator< const char*,
                                    mk_pred_functor< &is_any_of<'=',' ','\n','\t','\r','\0'> > > const_iterator;
  const_iterator begin() const { return const_iterator(p); }
  const_iterator end()   const { return const_iterator(); }
};

class attribute_value:
  base,
  public stringy<attribute_value> {
  // p points to the first character of the attribute's name (yes, NAME).
  explicit attribute_value(const char* p): base(p) {}
  friend class attribute;
public:
  attribute_value() {}
  class const_iterator:
    public boost::iterator_facade< const_iterator,
                                   char,
                                   std::forward_iterator_tag,
                                   char
                                 >,
    public safe_bool<const_iterator>
  {
    const char* p;
    char quote;
    const_iterator(const char* p_, char quote_): p(p_), quote(quote_) {}
    friend class attribute_value;
    friend class boost::iterator_core_access;

    char dereference() const { if (*p=='&') return decode_ref(p); return *p; }
    void increment() {
      if (*p=='&') {
        do { ++p; } while (!is_any_of<';','\0'>(*p));
      }
      ++p;
    }
    bool at_end() const { return *p==quote || *p=='\0'; }
    bool equal(const const_iterator& other) const {
      return (p == other.p)
          || (!p && other.at_end())
          || (!other.p && at_end());
    }

  public:
    const_iterator(): p(NULL) {}
    bool boolean_test() const { return p!=NULL && !at_end(); }
  };

  const_iterator begin() const;
  const_iterator end()   const { return const_iterator(); }
};

class attribute: base {
  // p points to the first character of the atttribute's name.
  explicit attribute(const char* p): base(p) {}
  friend class attribute_iterator;
public:
  attribute() {}
  attribute_name  name()  const { return attribute_name(p); }
  attribute_value value() const { return attribute_value(p); }
};

class attribute_iterator:
  base,
  public boost::iterator_facade< attribute_iterator,
                                 attribute,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 attribute
                               >,
  public safe_bool<attribute_iterator>
{
  // p points to the first character of the atttribute's name.
  // If there are no more attributes, it points to the >, / or ? at the end of 
  // the tag.  p==NULL is also an at-the-end iterator.
  // FIXME maybe better to set p=NULL when the last attribute has been passed.
  attribute_iterator(const char* p): base(p) {}
  friend class attributes;
  friend class boost::iterator_core_access;

  attribute dereference() const { return attribute(p); }
  void increment();
  bool at_end() const { return is_any_of<'>','/','?','\0'>(*p); }
  bool equal(const attribute_iterator& other) const {
    return (p == other.p)
        || (!p && other.at_end())
        || (!(other.p) && at_end()) ;
  }

public:
  attribute_iterator() {}
  bool boolean_test() const { return p!=NULL && !at_end(); }
};

class attributes:
  base,
  public containery<attributes> {
  // p points to the element's <.
  explicit attributes(const char* p): base(p) {}
  friend class element;
public:
  attributes() {}
  attribute_iterator begin() const;
  attribute_iterator end()   const { return attribute_iterator(); }
};

class text_node:
  base,
  public stringy<text_node> {
  // p points to the first character of the text (which may be the < of <![CDATA[).
  explicit text_node(const char* p): base(p) {}
  friend class node;
public:
  text_node() {}
  class const_iterator:
    public boost::iterator_facade< const_iterator,
                                   char,
                                   std::forward_iterator_tag,
                                   char
                                 >,
    public safe_bool<const_iterator>
  {
    // p points to a character within the text node, or is NULL if the 
    // iterator has reached the end.
    // Entity references are decoded.
    const char* p;
    bool in_cdata;
    const_iterator(const char* p_);
    friend class text_node;
    friend class boost::iterator_core_access;

    char dereference() const { if (*p=='&' && !in_cdata) return decode_ref(p); return *p; }
    void increment();
    bool equal(const const_iterator& other) const { return p == other.p; }

  public:
    const_iterator(): p(NULL) {}
    bool boolean_test() const { return p!=NULL; }
  };
  const_iterator begin() const { return const_iterator(p); }
  const_iterator end()   const { return const_iterator(); }
};

class node: base {
  // If the node is an element, p points to the < of its start or empty tag.
  // If the node is a comment, processing instruction or CDATA section, p also 
  // points to the < of its start tag.
  // If the node is a text node, p points to the first character of the text.
  explicit node(const char* p): base(p) {}
  friend class descendant_node_iterator;
  friend class document;  // for document::xmldecl_attrs().
  bool is_xmldecl()   const { return std::equal(p,p+5,"<?xml"); }
public:
  node() {}
  bool is_text_node() const { return *p!='<' || std::equal(p+1,p+9,"![CDATA["); }
  bool is_element()   const { return *p=='<' && !is_any_of<'!','?'>(*(succ(p))); }
  // text() can be called iff is_text_node(), which includes CDATA sections:
  text_node text() const { return text_node(p); }
  // elem() can be called iff is_element().
  element   elem() const; //  { return element(p); }  Break circular references - see below
  // If neither of is_text_node() or is_element() is true then the 
  // node is either a comment or a processing instruction (including the doctype); 
  // the content of these nodes is not accessible.
};

class descendant_node_iterator:
  base,
  public boost::iterator_facade< descendant_node_iterator,
                                 node,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 node
                               >,
  public safe_bool<descendant_node_iterator>
{
  // p points to the first character of the text or the < of the element's start or 
  // empty tag, or is NULL if there are no more nodes.
  // level is 1 for a child of the ancestor, 2 for a grandchild etc.
  unsigned int level_;
  descendant_node_iterator(const char* p, unsigned int level__): base(p), level_(level__) {}
  friend class descendant_nodes_t;
  friend class document;
  friend class boost::iterator_core_access;

  node dereference() const { return node(p); }
  void increment();
  bool equal(const descendant_node_iterator& other) const { return p == other.p; }

public:
  descendant_node_iterator() {}
  unsigned int level() const { return level_; }
  bool boolean_test() const { return p!=NULL; }
};

class descendant_nodes_t:
  base,
  public containery<descendant_nodes_t> {
  // p points to the ancestor element's <.
  explicit descendant_nodes_t(const char* p): base(p) {}
  friend class element;
public:
  descendant_nodes_t() {}
  descendant_node_iterator begin() const;
  descendant_node_iterator end()   const { return descendant_node_iterator(); }
};

class descendant_element_iterator:
  public boost::iterator_facade< descendant_element_iterator,
                                 element,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 element
                               >,
  public safe_bool<descendant_element_iterator>
{
  descendant_node_iterator i;
  explicit descendant_element_iterator(descendant_node_iterator i_): i(i_) {
    while (i && !i->is_element()) ++i;
  }
  friend class descendant_elements_t;
  friend class boost::iterator_core_access;

  element dereference() const; // { return i->elem(); }  See below - circuler reference.
  void increment() { do { ++i; } while (i && !i->is_element()); }
  bool equal(const descendant_element_iterator& other) const { return i == other.i; }

public:
  descendant_element_iterator(): i() {}
  unsigned int level() const { return i.level(); }
  bool boolean_test() const { return i; }
};

class descendant_elements_t:
  public containery<descendant_elements_t> {
  descendant_nodes_t n;
  explicit descendant_elements_t(descendant_nodes_t n_): n(n_) {}
  friend class element;
public:
  descendant_elements_t() {}
  descendant_element_iterator begin() const { return descendant_element_iterator(n.begin()); }
  descendant_element_iterator end()   const { return descendant_element_iterator(); }
};

class child_node_iterator:
  public boost::iterator_facade< child_node_iterator,
                                 node,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 node
                               >,
  public safe_bool<child_node_iterator>
{
  descendant_node_iterator i;
  explicit child_node_iterator(descendant_node_iterator i_): i(i_) {}
  friend class child_nodes_t;
  friend class boost::iterator_core_access;

  node dereference() const { return *i; }
  void increment() { do { ++i; } while (i && i.level()>1); }
  bool equal(const child_node_iterator& other) const { return i == other.i; }

public:
  child_node_iterator(): i() {}
  bool boolean_test() const { return i; }
};

class child_nodes_t:
  public containery<child_nodes_t> {
  descendant_nodes_t n;
  explicit child_nodes_t(descendant_nodes_t n_): n(n_) {}
  friend class element;
public:
  child_nodes_t() {}
  child_node_iterator begin() const { return child_node_iterator(n.begin()); }
  child_node_iterator end()   const { return child_node_iterator(); }
};

class child_element_iterator:
  public boost::iterator_facade< child_element_iterator,
                                 element,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 element
                               >,
  public safe_bool<child_element_iterator>
{
  descendant_node_iterator i;
  explicit child_element_iterator(descendant_node_iterator i_): i(i_) { while (i && !i->is_element()) ++i; }
  friend class child_elements_t;
  friend class boost::iterator_core_access;

  element dereference() const; // { return i->elem(); }  See below - circuler reference.
  void increment() { do { ++i; } while (i && (i.level()>1 || !i->is_element())); }
  bool equal(const child_element_iterator& other) const { return i == other.i; }

public:
  child_element_iterator(): i() {}
  bool boolean_test() const { return i; }
};

class child_elements_t:
  public containery<child_elements_t> {
  descendant_nodes_t n;
  explicit child_elements_t(descendant_nodes_t n_): n(n_) {}
  friend class element;
public:
  child_elements_t() {}
  child_element_iterator begin() const { return child_element_iterator(n.begin()); }
  child_element_iterator end()   const { return child_element_iterator(); }
};

template <typename NAME_T>
class descendant_element_named_iterator:
  public boost::iterator_facade< descendant_element_named_iterator<NAME_T>,
                                 element,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 element
                               >,
  public safe_bool< descendant_element_named_iterator<NAME_T> >
{
  descendant_node_iterator i;
  NAME_T name;
  bool name_matches() const; // { return i->elem().name()==name; }  Break circular refs.
  descendant_element_named_iterator(descendant_node_iterator i_, const NAME_T& name_):
    i(i_), name(name_) { while (i && (!i->is_element() || !name_matches())) ++i; }
  friend class descendant_elements_named_t<NAME_T>;
  friend class boost::iterator_core_access;

  element dereference() const; // { return i->elem(); }  See below - circuler reference.
  void increment() { do { ++i; } while (i && (!i->is_element() || !name_matches())); }
  bool equal(const descendant_element_named_iterator& other) const { return i == other.i; }

public:
  descendant_element_named_iterator(): i(), name() {}
  bool boolean_test() const { return i; }
  unsigned int level() const { return i.level(); }
};

template <typename NAME_T>
class descendant_elements_named_t:
  public containery< descendant_elements_named_t<NAME_T> > {
  descendant_nodes_t n;
  NAME_T name;
  descendant_elements_named_t(descendant_nodes_t n_, const NAME_T& name_): n(n_), name(name_) {}
  friend class element;
public:
  descendant_elements_named_t() {}
  descendant_element_named_iterator<NAME_T> begin() const { return descendant_element_named_iterator<NAME_T>(n.begin(),name); }
  descendant_element_named_iterator<NAME_T> end()   const { return descendant_element_named_iterator<NAME_T>(); }
};

template <typename NAME_T>
class child_element_named_iterator:
  public boost::iterator_facade< child_element_named_iterator<NAME_T>,
                                 element,
                                 std::forward_iterator_tag,  // FIXME could go back too
                                 element
                               >,
  public safe_bool< child_element_named_iterator<NAME_T> >
{
  descendant_node_iterator i;
  NAME_T name;
  bool name_matches() const; // { return i->elem().name()==name; } Break circular dependency
  child_element_named_iterator(descendant_node_iterator i_, const NAME_T& name_):
    i(i_), name(name_) { while (i && (i.level()>1 || !i->is_element() || !name_matches())) ++i; }
  friend class child_elements_named_t<NAME_T>;
  friend class boost::iterator_core_access;

  element dereference() const; // { return i->elem(); }  See below - circuler reference.
  void increment() { do { ++i; } while (i && (i.level()>1 || !i->is_element() || !name_matches())); }
  bool equal(const child_element_named_iterator& other) const { return i == other.i; }

public:
  child_element_named_iterator(): i(), name() {}
  bool boolean_test() const { return i; }
};

template <typename NAME_T>
class child_elements_named_t:
  public containery< child_elements_named_t<NAME_T> > {
  descendant_nodes_t n;
  NAME_T name;
  child_elements_named_t(descendant_nodes_t n_, const NAME_T& name_): n(n_), name(name_) {}
  friend class element;
public:
  child_elements_named_t() {}
  child_element_named_iterator<NAME_T> begin() const { return child_element_named_iterator<NAME_T>(n.begin(),name); }
  child_element_named_iterator<NAME_T> end()   const { return child_element_named_iterator<NAME_T>(); }
};

class element_name:
  base,
  public stringy<element_name> {
  // p points to the first character.
  explicit element_name(const char* p): base(succ(p)) {}
  friend class element;
public:
  element_name() {}
  typedef const_predicate_iterator< const char*,
                                    mk_pred_functor< &is_any_of<' ','/','>','\n','\t','\r','\0'> > > const_iterator;
  const_iterator begin() const { return const_iterator(p); }
  const_iterator end()   const { return const_iterator(); }
};

struct notfound {};  // exception thrown by element::must_*().

class element: base {
  // p points to the <.
  explicit element(const char* p): base(p) {}
  friend class node;
  friend class document;  // for document::xmldecl_attrs().
public:
  element() {}
  element_name            name()                const { return element_name(p); }
  attributes              attrs()               const { return attributes(p); }
  descendant_nodes_t      descendant_nodes()    const { return descendant_nodes_t(p); }
  descendant_elements_t   descendant_elements() const { return descendant_elements_t(descendant_nodes()); }
  child_nodes_t           child_nodes()         const { return child_nodes_t(descendant_nodes()); }
  child_elements_t        child_elements()      const { return child_elements_t(descendant_nodes()); }
  template <typename NAME_T>
  child_elements_named_t<NAME_T> child_elements_named(const NAME_T& name) const { return child_elements_named_t<NAME_T>(descendant_nodes(),name); }
  child_elements_named_t<const char*> child_elements_named(const char* name) const {
    return child_elements_named<const char*>(name);
  }
  template <typename NAME_T>
  descendant_elements_named_t<NAME_T> descendant_elements_named(const NAME_T& name) const { return descendant_elements_named_t<NAME_T>(descendant_nodes(),name); }
  std::string text() const {
    std::string t;
    for (descendant_node_iterator i = descendant_nodes().begin();
         i != descendant_nodes().end(); ++i) {
      if (i->is_text_node()) t.append(i->text().begin(),i->text().end());
    }
    return t;
  }
  descendant_elements_named_t<const char*> descendant_elements_named(const char* name) const {
    return descendant_elements_named<const char*>(name);
  }
  template <typename NAME_T>
  attribute must_attribute(const NAME_T& name) const {
    for (attribute_iterator i = attrs().begin();
         i != attrs().end(); ++i) {
      if (i->name() == name) return *i;
    }
    throw notfound();
  }
  attribute must_attribute(const char* name) const {
    return must_attribute<const char*>(name);
  }
  template <typename NAME_T>
  element must_child_element(const NAME_T& name) const {
    for (child_element_iterator i = child_elements().begin();
         i != child_elements().end(); ++i) {
      if (i->name() == name) return *i;
    }
    throw notfound();
  }
  element must_child_element(const char* name) const {
    return must_child_element<const char*>(name);
  }
};

class document {
  const char* begin;
public:
  document(const char* begin_, const char* /*end_*/): begin(begin_) {}
  attributes xmldecl_attrs() const; // This returns the attributes of the <?xml ... ?> XML declaration. 
                                    // If there is no XML declaration in the document, it is still safe to access this; it 
                                    // will be empty.
  element root() const;
};

struct file_base {
  file_base_impl* pimpl;
  file_base(std::string fn);
  ~file_base();
  const char* begin() const;
  const char* end() const;
};

class file:
  private file_base,
  public document {
public:
  file(std::string fn):
    file_base(fn),
    document(file_base::begin(),file_base::end())
  {}
};



inline element descendant_element_iterator::dereference()  const { return i->elem(); }
inline element child_element_iterator::dereference()       const { return i->elem(); }
template <typename NAME_T>
inline element child_element_named_iterator<NAME_T>::dereference() const { return i->elem(); }
template <typename NAME_T>
inline element descendant_element_named_iterator<NAME_T>::dereference() const { return i->elem(); }
template <typename NAME_T>
inline bool descendant_element_named_iterator<NAME_T>::name_matches() const { return i->elem().name()==name; }
template <typename NAME_T>
inline bool child_element_named_iterator<NAME_T>::name_matches() const { return i->elem().name()==name; }
inline element node::elem()                                const { return element(p); }



// Namespaces
// ----------
//
// Create a name_space object for each namespace that you're interested in.  Pass the 
// IRI of the namespace to the ctor, e.g.
//
//   rxml::name_space xhtml_ns("http://www.w3.org/1999/xhtml");
// 
// The name_space object tracks which prefixes are bound to the namespace.  While 
// traversing the document, update it at each element:
// 
//   xhtml_ns.push(e);
//   ....
//   xhtml_ns.pop();
//
// Or using a scoped convenience type:
//
//   {
//     rxml::namesapce::scope sc(e);
//     ...
//   }
//
// When looking at element names, compare them with namespace-qualified names as 
// follows:
//
//   if (e.name()==xhtml_ns("p")) ...
//
// Sometimes you may want to match multiple namespaces and treat them the same, 
// e.g. http://www.topografix.com/GPX/1/1 and http://www.topografix.com/GPX/1/0. 
// To do this, use a * wildcard.  This can only appear at the end of the IRI. 
// In this case, http://www.topografix.com/GPX/1/*.


class name_space {
  std::string iri;
  typedef std::vector<std::string> prefixes_t;
  prefixes_t prefixes;
  bool is_default;
  int empty_pushes;
  typedef boost::shared_ptr<name_space> ns_ptr;
  ns_ptr saved_state;

  template <typename ITER_T>
  bool match(const char* ncname, ITER_T qname_begin, ITER_T qname_end) const;

public:
  name_space() {}
  name_space(std::string iri_, bool is_default_=false):
    iri(iri_), is_default(is_default_), empty_pushes(0)
  {}

  class name {
    const name_space* ns_p;
    const char* ncname;
    name(const name_space& ns, const char* ncname_): ns_p(&ns), ncname(ncname_) {}
  public:
    name() {}
    template <typename NAME_T>
    bool operator==(NAME_T en) const {
      return ns_p->match(ncname,en.begin(),en.end());
    }
    template <typename NAME_T>
    bool operator!=(NAME_T en) const {
      return !operator==(en);
    }
    friend class name_space;
  };

  void push(element e);
  void pop();
  class scope {
    name_space& ns;
  public:
    scope(name_space& ns_, element e): ns(ns_) { ns.push(e); }
    ~scope() { ns.pop(); }
  };

  name operator()(const char* ncname) const { return name(*this,ncname); }
};


template <typename NAME_T>
inline bool operator==(NAME_T en, const name_space::name& m) {
  return m == en;
}

template <typename NAME_T>
inline bool operator!=(NAME_T en, const name_space::name& m) {
  return m != en;
}


template <typename ITER_T>
static bool match_one(std::string::const_iterator pbegin, std::string::const_iterator pend,
                      const char* ncname,
                      ITER_T qbegin, ITER_T qend)
{
  // Does prefix:ncname equal qname ?
  while (pbegin != pend) {
    if (qbegin==qend) return false;
    if (*qbegin!=*pbegin) return false;
    ++pbegin;
    ++qbegin;
  }
  if (qbegin==qend) return false;
  if (*qbegin!=':') return false;
  ++qbegin;
  while (*ncname) {
    if (qbegin==qend) return false;
    if (*qbegin!=*ncname) return false;
    ++ncname;
    ++qbegin;
  }
  if (qbegin!=qend) return false;
  return true;
}


template <typename ITER_T>
bool name_space::match(const char* ncname, ITER_T qname_begin, ITER_T qname_end) const
{
  if (is_default) {
    if (equal_ranges(qname_begin,qname_end,const_cstr_iterator(ncname),const_cstr_iterator())) return true;
  }
  // Traverse the prefixes backward, so that the most recent version of a re-used prefix
  // is found first.
  for (prefixes_t::const_reverse_iterator i = prefixes.rbegin();
       i != prefixes.rend(); ++i) {
    if (match_one(i->begin(),i->end(),ncname,qname_begin,qname_end)) return true;
  }
  return false;
}




};
};


#endif

