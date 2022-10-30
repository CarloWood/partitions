#include "sys.h"
#include "SetIteratorScatter.h"
#include "ElementPair.h"

//static
void SetIteratorScatter::initialize()
{
  for (ElementIndex i1 = Element::ibegin(); i1 != Element::iend(); ++i1)
  {
    for (ElementIndex i2 = i1 + 1; i2 != Element::iend(); ++i2)
    {
      ElementPair ep(i1, i2);
      int score_index = ep.score_index();
      Score const score = g_scores[score_index];
      Score zero;
      //Dout(dc::notice, "ep = " << ep << "; score = " << score << "; zero < score = " << std::boolalpha << (zero < score));
      if (zero < score)
        s_set23_to_score[Set(Element{i1}|Element{i2})] = score;
      for (ElementIndex i3 = i2 + 1; i3 != Element::iend(); ++i3)
      {
        ElementPair ep13(i1, i3);
        ElementPair ep23(i2, i3);
        int score_index13 = ep13.score_index();
        int score_index23 = ep23.score_index();
        Score score13 = g_scores[score_index13];
        Score score23 = g_scores[score_index23];
        Score score3 = score;
        score3 += score13;
        score3 += score23;
        if (zero < score3)
          s_set23_to_score[Set(Element{i1}|Element{i2}|Element{i3})] = score3;
      }
    }
  }
//  for (auto p : s_set23_to_score)
//    s_scores23.insert(std::make_pair(p.second, p.first));

//  for (auto s : s_scores23)
//    std::cout << s << '\n';
}

//static
std::map<Set, Score> SetIteratorScatter::s_set23_to_score;
//std::multimap<Score, Set> SetIteratorScatter::s_scores23;
std::once_flag SetIteratorScatter::s_initialize_flag;

SetIteratorScatter::SetIteratorScatter(Set orig) : m_orig(orig), m_method_A(std::make_shared<method_A_container_t>())
{
  std::call_once(s_initialize_flag, SetIteratorScatter::initialize);
  int orig_element_count = m_orig.element_count();
  // Don't try to iterate over a set that has less than 3 elements.
  ASSERT(orig_element_count > 2);
  for (auto element1 = m_orig.begin(); element1 != m_orig.end(); ++element1)
  {
    auto element2 = element1;
    for (++element2; element2 != m_orig.end(); ++element2)
    {
      auto element3 = (orig_element_count > 3) ? element2 : m_orig.end();
      for (++element3;; ++element3)
      {
        bool no_element3 = element3 == m_orig.end();
        Set set23{*element1};
        set23.add(Set{*element2});
        if (!no_element3)
          set23.add(Set{*element3});
        Score score = s_set23_to_score[set23];
        m_method_A->insert(std::make_pair(score, set23));
        if (no_element3)
          break;
      }
    }
  }
  m_current_A = m_method_A->begin();
}

SetIteratorScatter& SetIteratorScatter::operator++()
{
  if (++m_current_A == m_method_A->end())
    m_orig.clear();
  return *this;
}

Set SetIteratorScatter::operator*() const
{
  return m_current_A->second;
}

// Return amount the score goes down the next iteration.
Score SetIteratorScatter::score_difference() const
{
  // Don't call score_of_next_iteration() on end().
  ASSERT(!is_end());
  ASSERT(m_current_A != m_method_A->end());
  Score score = m_current_A->first;
  method_A_container_t::const_iterator next = m_current_A;
  if (++next == m_method_A->end())
    return {positive_inf};
  Score next_score = next->first;
  return score - next_score;
}

  Set m_orig;
  using method_A_container_t = std::multimap<Score, Set, std::greater<Score>>;
  std::shared_ptr<method_A_container_t> m_method_A;
  method_A_container_t::const_iterator m_current_A;

void SetIteratorScatter::print_on(std::ostream& os) const
{
  os << '{';
  os << "  m_orig: " << m_orig << '\n';
  for (method_A_container_t::const_iterator iter = m_method_A->begin(); iter != m_method_A->end(); ++iter)
    os << "  " << *iter << '\n';
  os << "  m_current_A = " << *m_current_A << '\n';
  os << '}';
}
