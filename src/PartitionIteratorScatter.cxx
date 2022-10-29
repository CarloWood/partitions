#include "sys.h"
#include "PartitionIteratorScatter.h"
#include "utils/VectorCompare.h"

struct GroupIteratorCompare
{
  bool operator()(std::pair<GroupIteratorScatter, GroupIndex> const& lhs, std::pair<GroupIteratorScatter, GroupIndex> const& rhs) const
  {
    return lhs.first.score_difference() < rhs.first.score_difference();
  }
};

PartitionIteratorScatter::PartitionIteratorScatter(Partition orig) : m_orig(orig)
{
  for (GroupIndex gi = m_orig.gbegin(); gi != m_orig.gend(); ++gi)
  {
    Group g = m_orig.group(gi);
    int element_count = g.element_count();
    if (element_count == 0)
      break;
    if (element_count > 2)
      m_group_iterators.emplace_back(std::make_pair(g, gi));
  }
  std::sort(m_group_iterators.begin(), m_group_iterators.end(), GroupIteratorCompare{});
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

  GroupIteratorScatter const& gis = ++(m_group_iterators.begin()->first);
  if (gis.is_end())
    m_group_iterators.clear();
  else
  {
    Score new_score_difference = gis.score_difference();
    auto end = std::find_if(m_group_iterators.begin() + 1, m_group_iterators.end(), [&](std::pair<GroupIteratorScatter, GroupIndex> const& v){ return v.first.score_difference() >= new_score_difference; });
    if (m_group_iterators.begin() + 1 != end)
      std::rotate(m_group_iterators.begin(), m_group_iterators.begin() + 1, end);
  }

  return *this;
}

Partition PartitionIteratorScatter::operator*() const
{
  Partition result = m_orig;
  GroupIndex feg = result.first_empty_group();
  for (auto& gisp : m_group_iterators)
  {
    GroupIteratorScatter const& gis = gisp.first;
    Group g = *gis;
    GroupIndex gi = gisp.second;
    result.remove_from(gi, g);
    result.add_to(feg++, g);
  }
  result.sort();
  return result;
}
