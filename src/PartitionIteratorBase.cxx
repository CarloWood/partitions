#include "sys.h"
#include "PartitionIteratorBase.h"

void PartitionIteratorSingleElement::increment()
{
  do
  {
    if (++m_to_group == (m_current_element_is_alone ? m_number_of_groups_original - 1 : m_number_of_groups_original + 1))
    {
      m_to_group.set_to_zero();
      if (++m_current_element_index == Element::iend())
      {
        set_to_end();
        break;
      }
      m_current_element_is_alone = m_original_partition.is_alone(m_current_element_index);
      m_from_group = m_original_partition.group_of(m_current_element_index);
    }
  }
  while (m_to_group == m_from_group);
}

void PartitionIteratorWholeGroup::increment()
{
  do
  {
    if (++m_from_group == m_first_empty_group)
    {
      m_from_group.set_to_zero();
      if (m_to_group == m_first_empty_group || ++m_to_group == GroupIndex{number_of_elements})
      {
        set_to_end();
        break;
      }
    }
  }
  while (m_to_group == m_from_group);
}
