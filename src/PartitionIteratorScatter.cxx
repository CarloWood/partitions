#include "sys.h"
#include "PartitionIteratorScatter.h"
#include "utils/VectorCompare.h"

struct SetIteratorCompare
{
  bool operator()(std::pair<SetIteratorScatter, SetIndex> const& lhs, std::pair<SetIteratorScatter, SetIndex> const& rhs) const
  {
    return lhs.first.score_difference() < rhs.first.score_difference();
  }
};

PartitionIteratorScatter::PartitionIteratorScatter(Partition orig) : m_orig(orig)
{
  for (SetIndex gi = m_orig.gbegin(); gi != m_orig.gend(); ++gi)
  {
    Set g = m_orig.set(gi);
    int element_count = g.element_count();
    if (element_count == 0)
      break;
    if (element_count > 2)
      m_set_iterators.emplace_back(std::make_pair(g, gi));
  }
  std::sort(m_set_iterators.begin(), m_set_iterators.end(), SetIteratorCompare{});
}

PartitionIteratorScatter& PartitionIteratorScatter::operator++()
{
  //    |       |       |
  //    v       v       v
  //  104(15)  90(25)  68(0)
  //   89(inf) 65(25)  68(13)
  //           40(inf) 55(inf)

  // 104, 90, 68
  // 104, 90, 68
  // 104, 90, 55
  //  89, 90, 55
  //  89, 65, 55
  //  89, 40, 55

  SetIteratorScatter const& gis = ++(m_set_iterators.begin()->first);
  if (gis.is_end())
    m_set_iterators.clear();
  else
  {
    Score new_score_difference = gis.score_difference();
    auto end = std::find_if(m_set_iterators.begin() + 1, m_set_iterators.end(), [&](std::pair<SetIteratorScatter, SetIndex> const& v){ return v.first.score_difference() >= new_score_difference; });
    if (m_set_iterators.begin() + 1 != end)
      std::rotate(m_set_iterators.begin(), m_set_iterators.begin() + 1, end);
  }

  return *this;
}

Partition PartitionIteratorScatter::operator*() const
{
  Partition result = m_orig;
  SetIndex feg = result.first_empty_set();
  for (auto& gisp : m_set_iterators)
  {
    SetIteratorScatter const& gis = gisp.first;
    Set g = *gis;
    SetIndex gi = gisp.second;
    result.remove_from(gi, g);
    result.add_to(feg++, g);
  }
  result.sort();
  return result;
}
