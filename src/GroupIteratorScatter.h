#pragma once

#include "Group.h"
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

class GroupIteratorScatter
{
 private:
  static std::map<Group, Score> s_group23_to_score;     // Required for method A.
//  static std::multimap<Score, Group> s_scores23;        // Required for method B.
  static std::once_flag s_initialize_flag;

  Group m_orig;
//  bool m_use_method_B;  // False: generate all results in advance and then sort them.
                        // True: run over all possible pairs/triplets in order of score.
  using method_A_container_t = std::multimap<Score, Group, std::greater<Score>>;
  std::shared_ptr<method_A_container_t> m_method_A;
  method_A_container_t::const_iterator m_current_A;

 public:
  // Create an end iterator.
  GroupIteratorScatter() : m_orig(elements_t{elements_t::mask_type{0}}) { }
  // Create the begin iterator.
  GroupIteratorScatter(Group orig);

  static void initialize();

  GroupIteratorScatter& operator++();

  bool is_end() const
  {
    return m_orig.empty();
  }

  Group operator*() const;

  Score score_difference() const;

  void print_on(std::ostream& os) const;
};
