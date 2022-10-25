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

constexpr ElementIndexPOD last_element_index{number_of_elements - 1};
constexpr ElementIndexPOD second_last_element_index{number_of_elements - 2};
constexpr ElementIndex last_element{last_element_index};
constexpr ElementIndex second_last_element{second_last_element_index};
constexpr elements_t::mask_type max_score_index = ElementPair{second_last_element, last_element}.get_pair().to_ulong();
std::vector<Score> g_scores(max_score_index + 1);
