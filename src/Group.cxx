#include "sys.h"
#include "Group.h"
#include "ElementPair.h"
#include <iostream>
#include <vector>

Score Group::score() const
{
  Score sum;
  if (m_elements.any() && !m_elements.is_single_bit())
  {
    for (auto bit_iter1 = m_elements.begin(); bit_iter1 != m_elements.end(); ++bit_iter1)
    {
      auto bit_iter2 = bit_iter1;
      ++bit_iter2;
      while (bit_iter2 != m_elements.end())
      {
        elements_t::mask_type score_index = (*bit_iter1)() | (*bit_iter2)();
        sum += g_scores[score_index];
        ++bit_iter2;
      }
    }
  }
  return sum;
}

void Group::print_on(std::ostream& os) const
{
  os << '{';
  for (ElementIndex i = Element::ibegin(); i != Element::iend(); ++i)
  {
    if (m_elements.test(Element{i}))
      os << Element{i};
  }
  os << '}';
}
