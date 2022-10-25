#include "sys.h"
#include "PartitionIterator.h"
#include "PartitionIteratorBase.h"

PartitionIterator::PartitionIterator(Partition const& orig, std::unique_ptr<PartitionIteratorBase>&& base) :
  m_base(std::move(base))
{
  m_base->kick_start();
}

PartitionIterator::PartitionIterator()
{
}

PartitionIterator::~PartitionIterator()
{
}

Partition PartitionIterator::operator*() const
{
  Partition p = m_base->original_partition();
  Group moved = m_base->moved_elements();
  assert(!m_base->from_group().undefined());
  assert(p.m_groups.ibegin() <= m_base->from_group());
  assert(m_base->from_group() < p.m_groups.iend());
  p.m_groups[m_base->from_group()].remove(moved);
  assert(!m_base->to_group().undefined());
  assert(p.m_groups.ibegin() <= m_base->to_group());
  assert(m_base->to_group() < p.m_groups.iend());
  p.m_groups[m_base->to_group()].add(moved);
  p.sort();
  return p;
}

PartitionIterator& PartitionIterator::operator++()
{
  m_base->increment();
  return *this;
}

bool operator!=(PartitionIterator const& lhs, PartitionIterator const& rhs)
{
  if (lhs.m_base && rhs.m_base)
    return lhs.m_base->unequal(*rhs.m_base);
  bool lhs_is_end = !lhs.m_base || lhs.m_base->is_end();
  bool rhs_is_end = !rhs.m_base || rhs.m_base->is_end();
  return lhs_is_end != rhs_is_end;
}
