#ifndef PARTITION_ITERATOR_BASE
#define PARTITION_ITERATOR_BASE

#include "Partition.h"

class PartitionIteratorBase
{
 protected:
  // The partition that we iterator over.
  Partition const m_original_partition;
  GroupIndex const m_number_of_groups_original;

  GroupIndex m_from_group{};            // The group that we're moving from.
  // Loop variable.
  GroupIndex m_to_group{0};             // The group that we're moving to.

 public:
  // Construct the 'end' iterator.
  PartitionIteratorBase() : m_to_group{} { }

  PartitionIteratorBase(Partition const& orig, GroupIndex from_group_index) :
    m_original_partition(orig),
    m_number_of_groups_original(orig.number_of_groups()),
    m_from_group(from_group_index) { }

  Partition const& original_partition() const
  {
    return m_original_partition;
  }

  GroupIndex from_group() const
  {
    return m_from_group;
  }

  GroupIndex to_group() const
  {
    return m_to_group;
  }

  void kick_start()
  {
    if (m_from_group == m_to_group)
      increment();
  }

  void set_to_end()
  {
    m_from_group.set_to_undefined();
  }

  bool is_end() const
  {
    return m_from_group.undefined();
  }

 virtual void increment() = 0;
 virtual Group moved_elements() const = 0;
 virtual bool unequal(PartitionIteratorBase const& rhs) const = 0;
};

#endif // PARTITION_ITERATOR_BASE
