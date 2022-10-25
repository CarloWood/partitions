#pragma once

#include "Element.h"
#include "Score.h"

class ElementPair
{
 private:
  elements_t m_element_pair;    // Has the two bits set that make up the pair; or none when this pair is undefined.

 public:
  // Construct an undefined element pair.
  ElementPair() = default;
  constexpr ElementPair(Element element1, Element element2) : m_element_pair(element1 | element2)
  {
    X_ASSERT(element1 != element2);
  }

  Score const& score()
  {
    elements_t::mask_type score_index = get_pair().to_ulong();
    return g_scores[score_index];
  }

  constexpr elements_t get_pair() const { return m_element_pair; }

  void print_on(std::ostream& os) const;
};
