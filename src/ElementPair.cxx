#include "sys.h"
#include "ElementPair.h"
#include <iostream>

void ElementPair::print_on(std::ostream& os) const
{
  os << '(' << m_element1 << ", " << m_element2 << ')';
}
