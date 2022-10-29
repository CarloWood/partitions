#pragma once

#include "Element.h"
#include "Score.h"

class ElementPair
{
 private:
  ElementIndex m_element1;
  ElementIndex m_element2;

 public:
  // Construct an undefined element pair.
  ElementPair() = default;
  constexpr ElementPair(ElementIndex element1, ElementIndex element2) : m_element1(element1), m_element2(element2)
  {
    X_ASSERT(element1 != element2);
  }

  Score const& score()
  {
    return g_scores[score_index()];
  }

  int score_index() const
  {
    return 64 * m_element1() + m_element2();
  }

  void print_on(std::ostream& os) const;
};
