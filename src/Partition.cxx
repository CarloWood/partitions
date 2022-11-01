#include "sys.h"
#include "Partition.h"
#include "PartitionIteratorSingleElement.h"
#include "PartitionIteratorWholeSet.h"
#include "PartitionIteratorBruteForce.h"
#include "PartitionIteratorScatter.h"
#include "PartitionTask.h"
#include <cassert>
#include <iostream>
#include <iomanip>

Partition::Partition(int number_of_elements) : m_sets(number_of_elements)
{
  for (auto i = m_sets.ibegin(); i != m_sets.iend(); ++i)
    m_sets[i] = Element{static_cast<char>('A' - (number_of_elements - 1 - i.get_value()))};
}

PartitionIterator Partition::end() const
{
  return {};
}

void Partition::print_sets() const
{
  char const* separator = "";
  for (ElementIndex e = element_ibegin(); e != element_iend(); ++e)
  {
    for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
    {
      if (m_sets[g].test(e))
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
  for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
  {
    if (m_sets[g].empty())
      break;
    os << prefix << m_sets[g];
    prefix = ", ";
  }
  os << '>';
}

SetIndex Partition::number_of_sets() const
{
  SetIndex g = m_sets.ibegin();
  while (g != m_sets.iend())
  {
    if (m_sets[g].empty())
      break;
    ++g;
  }
  return g;
}

bool Partition::is_alone(Element element) const
{
  for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
    if (m_sets[g].test(element))
      return m_sets[g].is_single_bit();
  // Never reached.
  assert(false);
  return false;
}

SetIndex Partition::set_of(Element element) const
{
  for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
    if (m_sets[g].test(element))
      return g;
  // Never reached.
  assert(false);
  return {};
}

SetIndex Partition::first_empty_set() const
{
  for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
    if (m_sets[g].empty())
      return g;
  return m_sets.iend();
}

Score Partition::score(PartitionTask const& partition_task) const
{
  Score sum;
  for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
  {
    if (m_sets[g].empty())
      break;
    sum += m_sets[g].score(partition_task);
  }
  return sum;
}

std::ostream& operator<<(std::ostream& os, Partition const& partition)
{
  partition.print_on(os);
  return os;
}

PartitionIteratorScatter Partition::sbegin()
{
  return {*this};
}

PartitionIteratorScatter Partition::send()
{
  return {};
}

//static
utils::RandomNumber Partition::s_random_number;

//static
Partition Partition::random(int number_of_elements)
{
  using partition_count_t = PartitionTask::partition_count_t;

  Partition top(number_of_elements, Set(Element('A')));
  int top_sets = 1;   // The current_root has 1 set.
  int top_elements = 1; // The current_root has 1 element.
  while (top_elements < number_of_elements)
  {
    Element const new_element('A' + top_elements);
    int depth = number_of_elements - top_elements;
    partition_count_t existing_set = PartitionTask::number_of_partitions(top_sets, depth - 1);
    partition_count_t new_set = PartitionTask::number_of_partitions(top_sets + 1, depth - 1);
    partition_count_t total = top_sets * existing_set + new_set;

#if 0
    std::cout << "top = " << top << '\n';
    std::cout << "top_sets = " << top_sets << '\n';
    std::cout << "top_elements = " << top_elements << '\n';
    std::cout << "depth = " << depth << '\n';
    std::cout << "existing_set = " << existing_set << '\n';
    std::cout << "new_set = " << new_set << '\n';
    std::cout << "total = " << total << '\n';
#endif

    std::uniform_int_distribution<partition_count_t> distr{0, total - 1};
    partition_count_t n = s_random_number.generate(distr);
//    std::cout << "random number = " << n << '\n';
    if (n >= top_sets * existing_set)
    {
      // Add new element to new set.
      top.add_to(SetIndex{top_sets}, new_element);
      ++top_sets;
    }
    else
    {
      // Add new element to existing set.
      top.add_to(SetIndex{static_cast<int>(n / existing_set)}, new_element);
    }
    ++top_elements;
  }
  return top;
}

Score Partition::find_local_maximum(PartitionTask const& partition_task)
{
  DoutEntering(dc::notice, "Partition::find_local_maximum(" << partition_task << ")");
  Score current_score = score(partition_task);
  constexpr int number_of_algorithms = 2;
  int no_improvement_count = 0;
  do
  {
    for (int algorithm = 0; algorithm < number_of_algorithms; ++algorithm)
    {
      Dout(dc::notice, "algorithm = " << algorithm);
      Score last_score;

      // Anticipate that the current algorithm will fail to find any improvement.
      ++no_improvement_count;
      // Run algorithm 'algorithm'.
      do
      {
        Dout(dc::notice, *this << " = " << current_score);
        last_score = current_score;

        for (auto neighboring_partition_iterator =
            algorithm == 0 ? begin<PartitionIteratorWholeSet>() : begin<PartitionIteratorSingleElement>() ;
             neighboring_partition_iterator != end();
             ++neighboring_partition_iterator)
        {
          Dout(dc::notice, "neighboring_partition_iterator = " << neighboring_partition_iterator);
          Partition neighboring_partition = *neighboring_partition_iterator;
          Dout(dc::notice, "neighboring_partition = " << neighboring_partition);
          Score neighboring_score = neighboring_partition.score(partition_task);
          Dout(dc::notice, "neighboring_score = " << neighboring_score);
          if (neighboring_score > current_score)
          {
            m_sets = neighboring_partition.m_sets;
            current_score = neighboring_score;
            no_improvement_count = 1;
          }
        }
      }
      while (current_score > last_score);       // If 'algorithm' found an improvement, run the same algorithm again.

      // If none of the algorithms could improve, terminate the program.
      if (no_improvement_count == number_of_algorithms)
        break;
    }
  }
  while (no_improvement_count < number_of_algorithms);
  return current_score;
}
