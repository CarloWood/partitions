#include "sys.h"
#include "PartitionIteratorWholeGroup.h"

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
