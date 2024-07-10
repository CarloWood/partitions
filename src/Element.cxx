#include "sys.h"
#include "Element.h"
#include <iostream>

void Element::print_on(std::ostream& os) const
{
  os << m_name;
}

std::ostream& operator<<(std::ostream& os, Element const& element)
{
  element.print_on(os);
  return os;
}
