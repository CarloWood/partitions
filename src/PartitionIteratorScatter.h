#pragma once

#include "Partition.h"
#include "PairTripletIteratorScatter.h"
#include "utils/MultiLoop.h"
#include <vector>

// {ABCF}, {DG}, {EHI}
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
//
// {AB}, {CF}, {DG}, {EH}, {I}
// {AB}, {CF}, {DG}, {EI}, {H}
// {AB}, {CF}, {DG}, {E}, {HI}
// {AC}, {BF}, {DG}, {EH}, {I}
// {AC}, {BF}, {DG}, {EI}, {H}
// {AC}, {BF}, {DG}, {E}, {HI}
// {AF}, {BC}, {DG}, {EH}, {I}
// {AF}, {BC}, {DG}, {EI}, {H}
// {AF}, {BC}, {DG}, {E}, {HI}
// {ABC}, {F}, {DG}, {EH}, {I}
// {ABC}, {F}, {DG}, {EI}, {H}
// {ABC}, {F}, {DG}, {E}, {HI}
// {ABD}, {C}, {DG}, {EH}, {I}
// {ABD}, {C}, {DG}, {EI}, {H}
// {ABD}, {C}, {DG}, {E}, {HI}
// {ACD}, {B}, {DG}, {EH}, {I}
// {ACD}, {B}, {DG}, {EI}, {H}
// {ACD}, {B}, {DG}, {E}, {HI}
// {BCD}, {A}, {DG}, {EH}, {I}
// {BCD}, {A}, {DG}, {EI}, {H}
// {BCD}, {A}, {DG}, {E}, {HI}

struct SetIteratorCompare;

class PartitionIteratorScatter
{
 public:
  static constexpr int total_loop_count_limit = 100;

 private:
  friend struct SetIteratorCompare;
  struct SetIterator
  {
    PairTripletIteratorScatter m_pair_triplet_iterator;
    SetIndex m_set_index;
    int m_loop_count;

    SetIterator(PartitionTask const& partition_task, Set set, SetIndex set_index, int loop_count) :
      m_pair_triplet_iterator(partition_task, set), m_set_index(set_index), m_loop_count(loop_count) { }

    void reset()
    {
      m_pair_triplet_iterator.reset();
    }

    void print_on(std::ostream& os) const;
  };

  Partition m_orig;
  std::vector<SetIterator> m_pair_triplet_iterators;
  MultiLoop m_pair_triplet_counters;

 private:
  bool increase_loop_count();

 public:
  // Create an end iterator.
  PartitionIteratorScatter() = default;
  // Create the begin iterator.
  PartitionIteratorScatter(PartitionTask const& partition_task, Partition orig);

  PartitionIteratorScatter& operator++();

  bool is_end() const
  {
    return m_pair_triplet_counters.finished();
  }

  Partition operator*() const;

  void print_on(std::ostream& os) const;
};
