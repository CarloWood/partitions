#include "sys.h"
#include "PartitionIteratorBruteForce.h"

PartitionIteratorBruteForce::PartitionIteratorBruteForce(int) : m_multiloop(number_of_elements), m_loop_value_count{}
{
  for (; !m_multiloop.finished(); m_multiloop.next_loop())
  {
    for (;;)
    {
      // A loop ends when moving the element to the next set would leave its current set empty.
      if (m_multiloop() > 0 && --m_loop_value_count[m_multiloop() - 1] == 0)
        break;
      ++m_loop_value_count[m_multiloop()];

      if (m_multiloop.inner_loop())             // Most inner loop.
      {
        // We're ready to execute operator*/operator++ while in the inner loop.
        return;
      }
      m_multiloop.start_next_loop_at(0);
    }
  }
}

PartitionIteratorBruteForce& PartitionIteratorBruteForce::operator++()
{
  m_multiloop.start_next_loop_at(0);
  do
  {
    for (;;)
    {
      if (m_multiloop() > 0 && --m_loop_value_count[m_multiloop() - 1] == 0)
        break;
      ++m_loop_value_count[m_multiloop()];
      if (m_multiloop.inner_loop())             // Most inner loop.
        return *this;
      m_multiloop.start_next_loop_at(0);
    }
    m_multiloop.next_loop();
  }
  while (!m_multiloop.finished());
  return *this;
}

Partition PartitionIteratorBruteForce::operator*() const
{
  utils::Array<Set, number_of_elements, SetIndex> sets;
  for (SetIndex g = sets.ibegin(); g != sets.iend(); ++g)
    sets[g] = Set{elements_t{0}};
  for (ElementIndex l{Element::s_ibegin}; l < Element::s_iend; ++l)
    sets[SetIndex{m_multiloop[l()]}].add(Element{l});
  return {sets};
}
