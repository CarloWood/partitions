#include "sys.h"
#include "Partition.h"
#include "PartitionIteratorBruteForce.h"
#include <cassert>
#include <iostream>

Partition::Partition()
{
  for (auto i = m_groups.ibegin(); i != m_groups.iend(); ++i)
    m_groups[i] = Element{static_cast<char>('A' - (number_of_elements - 1 - i.get_value()))};
}

PartitionIterator Partition::end() const
{
  return {};
}

void Partition::print_groups() const
{
  char const* separator = "";
  for (ElementIndex e = Element::ibegin(); e != Element::iend(); ++e)
  {
    for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
    {
      if (m_groups[g].test(e))
      {
        std::cout << separator << Element{e} << "=" << g.get_value();
        separator = ", ";
      }
    }
  }
}

void Partition::print_on(std::ostream& os) const
{
  os << '<';
  char const* prefix = "";
  for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
  {
    if (m_groups[g].empty())
      break;
    os << prefix << m_groups[g];
    prefix = ", ";
  }
  os << '>';
}

GroupIndex Partition::number_of_groups() const
{
  GroupIndex g = m_groups.ibegin();
  while (g != m_groups.iend())
  {
    if (m_groups[g].empty())
      break;
    ++g;
  }
  return g;
}

bool Partition::is_alone(Element element) const
{
  for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
    if (m_groups[g].test(element))
      return m_groups[g].is_single_bit();
  // Never reached.
  assert(false);
  return false;
}

GroupIndex Partition::group_of(Element element) const
{
  for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
    if (m_groups[g].test(element))
      return g;
  // Never reached.
  assert(false);
  return {};
}

GroupIndex Partition::first_empty_group() const
{
  for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
    if (m_groups[g].empty())
      return g;
  return m_groups.iend();
}

Score Partition::score() const
{
  Score sum;
  for (GroupIndex g = m_groups.ibegin(); g != m_groups.iend(); ++g)
  {
    if (m_groups[g].empty())
      break;
    sum += m_groups[g].score();
  }
  return sum;
}

std::ostream& operator<<(std::ostream& os, Partition const& partition)
{
  partition.print_on(os);
  return os;
}

//static
PartitionIteratorBruteForce Partition::bbegin()
{
  return {0};
}

//static
PartitionIteratorBruteForce Partition::bend()
{
  return {};
}
