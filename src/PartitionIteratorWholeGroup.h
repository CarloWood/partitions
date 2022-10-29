#pragma once

#include "PartitionIteratorBase.h"

class PartitionIteratorWholeGroup final : public PartitionIteratorBase
{
 private:
  // Second loop variable.
  GroupIndex const m_first_empty_group;

 public:
  // Create an end iterator.
  PartitionIteratorWholeGroup() = default;

  // Create a begin iterator.
  PartitionIteratorWholeGroup(Partition const& orig) : PartitionIteratorBase(orig, GroupIndex{0}), m_first_empty_group(orig.first_empty_group())
  {
  }

 private:
  Group moved_elements() const override
  {
    return m_original_partition.group(m_from_group);
  }

  void increment() override;

  bool unequal(PartitionIteratorBase const& rhs) const override
  {
    return m_from_group != rhs.from_group() || m_to_group != rhs.to_group();
  }
};
