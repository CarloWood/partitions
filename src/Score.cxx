#include "sys.h"
#include "Score.h"
#include "Element.h"
#include "ElementPair.h"
#include <iostream>

void Score::print_on(std::ostream& os) const
{
  if (m_number_of_negative_inf == 1 && m_number_of_positive_inf == 0 && m_value == 0.0)
    os << "-inf";
  else if (m_number_of_negative_inf == 0 && m_number_of_positive_inf == 1 && m_value == 0.0)
    os << "+inf";
  else
  {
    os << m_value;
    if (m_number_of_positive_inf)
      os << " + inf * " << m_number_of_positive_inf;
    if (m_number_of_negative_inf)
      os << " - inf * " << m_number_of_negative_inf;
  }
}

// See ElementPair::score_index().
constexpr int scores_size = 64 * (number_of_elements - 1) + number_of_elements;
std::vector<Score> g_scores(scores_size);
