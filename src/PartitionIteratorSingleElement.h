#pragma once

#include "PartitionIteratorBase.h"

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
