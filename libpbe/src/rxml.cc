// src/rxml.hh
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

#include "rxml.hh"

#include "FileDescriptor.hh"
#include "MappedFile.hh"
#include "predicates.hh"
#include "succpred.hh"
#include "strto.hh"

using namespace std;


namespace pbe {
namespace rxml {


template <char c1>
static const char* skip_to(const char* p)
{
  // Return a pointer to the next c1 at or after p.
  while (!is_any_of<c1>(*p)) ++p;
  return p;
}

template <char c1, char c2>
static const char* skip_to(const char* p)
{
  // Return a pointer to the next c1 or c2 at or after p.
  while (!is_any_of<c1,c2>(*p)) ++p;
  return p;
}

template <char c1, char c2, char c3>
static const char* skip_to(const char* p)
{
  // Return a pointer to the next c1 or c2 or c3 at or after p.
  while (!is_any_of<c1,c2,c3>(*p)) ++p;
  return p;
}

template <char c1, char c2, char c3, char c4>
static const char* skip_over(const char* p)
{
  // Return a pointer to the next character not c1 or c2 or c3 or c4 at or after p.
  while (is_any_of<c1,c2,c3,c4>(*p)) ++p;
  return p;
}


bool xml_isspace(char c)
{
  // XML 1.1 section 2.3 defines whitespace as #x20 (space), #x9 (tab), #xD (cr), #xA (lf).
  // These are all "safe" in UTF-8, ASCII, and ISO-8859-*.
  // No need to worry about things like non-breaking-space here.
  return is_any_of<' ','\n','\t','\r'>(c);
}

template <char c1, char c2, char c3, char c4>
static const char* skip_to_space_or(const char* p)
{
  while (!xml_isspace(*p) && !is_any_of<c1,c2,c3,c4>(*p)) ++p;
  return p;
}

static const char* skip_over_space(const char* p)
{
  while (xml_isspace(*p)) ++p;
  return p;
}

static const char* skip_to_attribute_value(const char* p)
{
  // Return a pointer to the next ' or " at or after p.
  return skip_to<'"','\'','\0'>(p);
}

static const char* skip_over_attribute_value(const char* p)
{
  // p points to the ' or " at the start of an attribute value.
  // Return a pointer to the character after the corresponding ' or " 
  // at the end of the attribute value.
  if (*p=='"') {
    return succ(skip_to<'"','\0'>(succ(p)));
  } else {
    return succ(skip_to<'\'','\0'>(succ(p)));
  }
}

static const char* skip_to_end_of_tag(const char* p)
{
  // p points to the < at the start of a tag.
  // For a start or end tag, return a pointer to the > at the end.
  // For an empty tag, return a pointer to the / at the end.
  // Also works for the xmldecl element, for which it returns a pointer to the ? at the end.
  p = p+2;                                 // Skip over the < and any / or ?.
  // FIXME that misses any NULL at p or p+1.
  p = skip_to_space_or<'>','/','?','\0'>(p);    // Skip over the (rest of the) tag's name.
  while (1) {  // Loop over the attributes.
    p = skip_over_space(p);                // Skip over spaces.
    if (is_any_of<'/','>','?','\0'>(*p)) break; // At end of tag.
    p = skip_to_attribute_value(p);        // Skip over attribute name and =.
    p = skip_over_attribute_value(p);      // Skip over attribute value.
  }
  return p;
}

static const char* skip_to_end_of_comment(const char* p)
{
  // p points to the < at the start of a comment.
  // Return a pointer to the > at the end.
  // There may be other >s within the comment; we need to find the pattern -->.
  int dashes = 0;
  while (1) {
    if (*p == '-') {
      ++dashes;
    } else {
      if (*p == '>' && dashes == 2) {
        return p;
      }
      if (*p == '\0') {
        return p;
      }
      dashes = 0;
    }
    ++p;
  }
}

static const char* skip_to_end_of_cdata(const char* p)
{
  // p points to the < at the start of <![CDATA[.
  // Return a pointer to the > at the end.
  // There may be other >s within the cdata; we need to find the pattern ]]>.
  int brackets = 0;
  while (1) {
    if (*p == ']') {
      ++brackets;
    } else {
      if (*p == '>' && brackets >= 2) {
        return p;
      }
      if (*p == '\0') {
        return p;
      }
      brackets = 0;
    }
    ++p;
  }
}

static const char* skip_to_end_of_doctype(const char* p)
{
  // p points to the < of <!DOCTYPE, which is matched by a plain >.
  return skip_to<'>','\0'>(p);
}


char decode_ref(const char* p)
{
  if (std::equal(p,p+4,"&lt;"))   return '<';
  if (std::equal(p,p+4,"&gt;"))   return '>';
  if (std::equal(p,p+5,"&amp;"))  return '&';
  if (std::equal(p,p+6,"&quot;")) return '"';
  if (std::equal(p,p+6,"&apos;")) return '\'';
  if (std::equal(p,p+3,"&#x")) {
    const char* semi = p+3;
    while (!is_any_of<';','\0'>(*semi)) ++semi;
    return hex_strtol(p+3,semi);
  }
  if (std::equal(p,p+2,"&#")) {
    const char* semi = p+2;
    while (!is_any_of<';','\0'>(*semi)) ++semi;
    return strtol(p+2,semi);
  }
  throw UnknownEntityReference();
}


void attribute_iterator::increment()
{
  // p points to the start of an attribute name.
  // Advance it to point to the start of the next attribute's name, or 
  // if there are no more attributes to the / or > at the end of the tag.
  p = skip_to_attribute_value(p);    // Skip name and =
  p = skip_over_attribute_value(p);  // Skip value
  p = skip_over_space(p);            // Skip space
  // FIXME see below about skipping ? and /.
}


attribute_value::const_iterator attribute_value::begin() const
{
  // p points to the start of the attribute's name.
  const char* q = skip_to_attribute_value(p);  
  char quote = *q;
  if (quote=='\0') throw UnexpectedEndOfDocument();
  ++q;
  return const_iterator(q,quote);
}


attribute_iterator attributes::begin() const
{
  // p points to the < at the start of the tag.
  const char* q = succ(succ(p));  // Skip < and any / or ?.
  // FIXME that misses any NULL at p or p+1.
                                  // For a start tag this skips the first character of the name, so 
                                  // we could already be pointing to the character after the name. 
                                  // That's OK.
  q = skip_to_space_or<'>','/','?','\0'>(q);  // Skip over the tag's name.
  q = skip_over_space(q);                // Skip over spaces.
  // q now points to the start of an attribute's name, or to the >, / or ? at the end of the tag.
  // FIXME can we ignore ? here, and go to the xmldecl's > ?
  // FIXME can we also ignore / and go to the > ?
  return attribute_iterator(q);
}


enum frag_type { ft_text, ft_cdata,
                 ft_start_or_empty_tag, ft_endtag,
                 ft_comment, ft_xmldecl, ft_doctype };

struct UnsupportedTag {};

static frag_type identify_frag(const char* p)
{
  if (*p != '<') return ft_text;
  ++p;
  switch (*p) {
    case '/': return ft_endtag;
    case '!': {
      ++p;
      if (*p == '-' && *(succ(p)) == '-') return ft_comment;
      if (std::equal(p,p+7,"[CDATA[")) return ft_cdata;
      if (std::equal(p,p+7,"DOCTYPE")) return ft_doctype;
      throw UnsupportedTag();
    }
    case '?': {
      ++p;
      if (std::equal(p,p+3,"xml")) return ft_xmldecl;
      throw UnsupportedTag();
    }
    default: {
      return ft_start_or_empty_tag;
    }
  }
}

static const char* skip_ft(const char* p, frag_type ft)
{
  switch (ft) {
    case ft_text:
      p = skip_to<'<','\0'>(p);
      break;
    case ft_cdata:
      p = skip_to_end_of_cdata(p);
      ++p;
      break;
    case ft_start_or_empty_tag:
      p = skip_to_end_of_tag(p);
      if (*p=='/') ++p;
      ++p;
      break;
    case ft_endtag:
      p = skip_to_end_of_tag(p);
      ++p;
      break;
    case ft_comment:
      p = skip_to_end_of_comment(p);
      ++p;
      break;
    case ft_xmldecl:
      p = skip_to_end_of_tag(p);
      p += 2;
      // FIXME that fails to check for NULL.
      break;
    case ft_doctype:
      p = skip_to_end_of_doctype(p);
      ++p;
      break;
  }
  return p;
}


descendant_node_iterator descendant_nodes_t::begin() const
{
  // p points to the < of the ancestor element's start or empty tag.
  // We need to find the start of the first descendant node, either element or text node,
  // or NULL if there are no descendant nodes.
  const char* q = skip_to_end_of_tag(p);   // Skip to / or > at end of tag.
  if (*q == '/') {                         // Empty element; no descendant nodes.
    return descendant_node_iterator();
  }
  ++q;                                     // Skip over >
  // FIXME NULL checking?
  if (*q=='<' && *(succ(q))=='/') {        // End tag - element is empty.
    return descendant_node_iterator();
  }
  return descendant_node_iterator(q,1);
}


void descendant_node_iterator::increment()
{
  frag_type ft = identify_frag(p);
  p = skip_ft(p,ft);
  if (ft == ft_start_or_empty_tag) {
    if (*(p-2) != '/') ++level_;
  }

  while (1) {
    ft = identify_frag(p);
    if (ft != ft_endtag) {
      return;
    }
    --level_;
    if (level_==0) {
      p = NULL;
      return;
    }
    p = skip_ft(p,ft);  // FIXME we know it's an end tag.
  }
}


text_node::const_iterator::const_iterator(const char* p_):
  p(p_), in_cdata(false)
{
  if (std::equal(p,p+9,"<![CDATA[")) {
    in_cdata = true;
    p += 9;
    if (std::equal(p,p+3,"]]>")) {
      p = NULL;
    }
  }
}


void text_node::const_iterator::increment()
{
  if (!in_cdata && *p=='&') {
    p = skip_to<';','\0'>(p);
    if (*p=='\0') throw UnexpectedEndOfDocument();
  }
  ++p;
  if (*p=='\0') throw UnexpectedEndOfDocument();
  if (in_cdata) {
    if (std::equal(p,p+3,"]]>")) {
      p = NULL;
    }
  } else {
    if (*p=='<') {
      p = NULL;
    }
  }
}


const char dummy_xmldecl[] = "<?xml>";

attributes document::xmldecl_attrs() const
{
  element e;
  node n(begin);
  if (n.is_xmldecl()) {
    e = n.elem();
  } else {
    e = element(dummy_xmldecl);
  }
  return e.attrs();
}


element document::root() const
{
  descendant_node_iterator i(begin,1);
  while (!i->is_element()) ++i;
  return i->elem();
}


struct file_base_impl {

  FileDescriptor fd;
  MappedFile mf;

  file_base_impl(string fn):
    fd(fn, pbe::FileDescriptor::read_only),
    mf(fd,fd.file_length(),pbe::FileDescriptor::read_write,0,true)
  {
    // Maybe we can somehow make only the last page COW.
    mf.ref<char>(fd.file_length()-1) = 0;
  }

  const char* begin() const {
    return mf.ptr<const char>(0);
  }

  const char* end() const {
    return mf.ptr<const char>(mf.length);
  }

};


file_base::file_base(string fn):
  pimpl(new file_base_impl(fn))
{}

file_base::~file_base()
{
  delete pimpl;
}

const char* file_base::begin() const
{
  return pimpl->begin();
}

const char* file_base::end() const
{
  return pimpl->end();
}


static bool wildcard_match(attribute_value val, const string& iri)
{
  attribute_value::const_iterator i = val.begin();
  string::const_iterator j = iri.begin();
  while (1) {
    if (i==val.end() && j==iri.end()) return true;
    if (j==iri.end()) return false;
    if (*j=='*') return true;
    if (i==val.end()) return false;
    if (*i != *j) return false;
    ++i;
    ++j;
  }
}

void name_space::push(element e)
{
  bool new_is_default = is_default;
  prefixes_t new_prefixes;
  for (attribute_iterator i = e.attrs().begin();
       i != e.attrs().end(); ++i) {
    bool name_is_xmlns = i->name()=="xmlns";
    if (wildcard_match(i->value(),iri)) {
      if (name_is_xmlns) {
        new_is_default = true;
      } else {
        std::string n = i->name().str();
        if (starts_with(n,"xmlns:")) {
          new_prefixes.push_back(std::string(n.begin()+6,n.end()));
        }
      }
    } else {
      if (name_is_xmlns) new_is_default = false;
    }
  }
  if (new_prefixes.empty() && new_is_default==is_default) {
    ++empty_pushes;
    return;
  }
  ns_ptr copy_p(new name_space(*this));
  saved_state = copy_p;
  empty_pushes = 0;
  is_default = new_is_default;
  prefixes.insert(prefixes.end(),new_prefixes.begin(),new_prefixes.end());
}


void name_space::pop()
{
  if (empty_pushes>0) {
    --empty_pushes;
    return;
  }

  ns_ptr restore = saved_state;
  *this = *restore;
}



};
};



