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

class PartitionIteratorSingleElement final : public PartitionIteratorBase
{
 private:
  // Second loop variable.
  ElementIndex m_current_element_index{Element::ibegin()};      // 0...N-1
  // Cached information about the current element.
  bool m_current_element_is_alone{};            // Whether or not it is the only elment in that group (in the original).

 public:
  // Create an end iterator.
  PartitionIteratorSingleElement() : m_current_element_index(Element::ibegin()) { }

  // Create a begin iterator.
  PartitionIteratorSingleElement(Partition const& orig) :
    PartitionIteratorBase(orig, orig.group_of(Element::ibegin())),
    m_current_element_is_alone(orig.is_alone(m_current_element_index))
  {
  }

 private:
  Group moved_elements() const override
  {
    return Element{m_current_element_index};
  }

  void increment() override;

  bool unequal(PartitionIteratorBase const& rhs) const override
  {
    return m_current_element_index != static_cast<PartitionIteratorSingleElement const&>(rhs).m_current_element_index || m_to_group != rhs.to_group();
  }
};

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

#endif // PARTITION_ITERATOR_BASE
