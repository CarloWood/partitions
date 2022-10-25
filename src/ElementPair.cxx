#include "sys.h"
#include "ElementPair.h"
#include <iostream>

void ElementPair::print_on(std::ostream& os) const
{
  for (Element element : m_element_pair)
    os << element;
}
