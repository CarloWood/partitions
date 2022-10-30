#pragma once

#include "Set.h"
#include "Score.h"
#include <map>
#include <mutex>
#include <memory>

// {ABCF}
//
// {AB}
// {AC}
// {AF}
// {BC}
// {BF}
// {CF}
// {ABC}
// {ABF}
// {ACF}
// {BCF}

class SetIteratorScatter
{
 private:
  static std::map<Set, Score> s_set23_to_score;     // Required for method A.
//  static std::multimap<Score, Set> s_scores23;        // Required for method B.
  static std::once_flag s_initialize_flag;

  Set m_orig;
//  bool m_use_method_B;  // False: generate all results in advance and then sort them.
                        // True: run over all possible pairs/triplets in order of score.
  using method_A_container_t = std::multimap<Score, Set, std::greater<Score>>;
  std::shared_ptr<method_A_container_t> m_method_A;
  method_A_container_t::const_iterator m_current_A;

 public:
  // Create an end iterator.
  SetIteratorScatter() : m_orig(elements_t{elements_t::mask_type{0}}) { }
  // Create the begin iterator.
  SetIteratorScatter(Set orig);

  static void initialize();

  SetIteratorScatter& operator++();

  bool is_end() const
  {
    return m_orig.empty();
  }

  Set operator*() const;

  Score score_difference() const;

  void print_on(std::ostream& os) const;
};
