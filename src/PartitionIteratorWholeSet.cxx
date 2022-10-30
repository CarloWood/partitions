#include "sys.h"
#include "PartitionIteratorWholeSet.h"

void PartitionIteratorWholeSet::increment()
{
  do
  {
    if (++m_from_set == m_first_empty_set)
    {
      m_from_set.set_to_zero();
      if (m_to_set == m_first_empty_set || ++m_to_set == SetIndex{number_of_elements})
      {
        set_to_end();
        break;
      }
    }
  }
  while (m_to_set == m_from_set);
}
