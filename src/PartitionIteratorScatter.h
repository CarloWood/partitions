#pragma once

#include "Partition.h"
#include "GroupIteratorScatter.h"
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

class PartitionIteratorScatter
{
 private:
  Partition m_orig;
  std::vector<std::pair<GroupIteratorScatter, GroupIndex>> m_group_iterators;

 public:
  // Create an end iterator.
  PartitionIteratorScatter() { }
  // Create the begin iterator.
  PartitionIteratorScatter(Partition orig);

  PartitionIteratorScatter& operator++();

  bool is_end() const
  {
    return m_group_iterators.empty();
  }

  Partition operator*() const;
};
