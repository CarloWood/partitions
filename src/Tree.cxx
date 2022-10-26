#include "sys.h"
#include "Tree.h"
#include "utils/iomanip.h"
#include <iostream>

class Indentation : public utils::iomanip::Sticky
{
 private:
  static utils::iomanip::Index s_index;

 public:
  Indentation(long iword_value) : Sticky(s_index, iword_value) { }

  static long get_iword_value(std::ostream& os) { return get_iword_from(os, s_index); }
};

//static
utils::iomanip::Index Indentation::s_index;

void Tree::print_on(std::ostream& os) const
{
  long indentation = Indentation::get_iword_value(os);
  std::string indentation_str(indentation, ' ');
  os << indentation_str << '{';
  os << indentation_str << m_element << m_group << ':' << m_count << '=' << '\n';
  os << Indentation(indentation + 2);
  char const* separator = "";
  for (auto const& child : m_children)
  {
    os << separator << child;
    separator = ",\n";
  }
  if (!m_children.empty())
    os << '\n';
  os << indentation_str << "}";
  os << Indentation(indentation);
}
