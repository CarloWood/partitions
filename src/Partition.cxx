#include "sys.h"
#include "Partition.h"
#include "PartitionIteratorSingleElement.h"
#include "PartitionIteratorWholeGroup.h"
#include "PartitionIteratorBruteForce.h"
#include "PartitionIteratorScatter.h"
#include <cassert>
#include <iostream>
#include <iomanip>

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

PartitionIteratorScatter Partition::sbegin()
{
  return {*this};
}

PartitionIteratorScatter Partition::send()
{
  return {};
}

#if 0

                                             {A}
               {AB}                                                            {A,B}
    {ABC}                {AB,C}          |          {AC,B}                     {A,BC}                          {A,B,C}
{ABCD} {ABC,D} | {ABD,C} {AB,CD} {AB,C,D} | {ACD,B} {AC,BD} {AC,B,D} | {AD,BC} {A,BCD} {A,BC,D} | {AD,B,C} {A,BD,C} {A,B,CD} {A,B,C,D}

Table "1"

depth (d)
|
v   1   2   3   4   5 ...     <-- groups (g)
0   1   0   0   0   0 ...  1
1   1   1   0   0   0 ...  2
2   1   3   1   0   0 ...  5
3   1   7   6   1   0 ... 15
4   1  15  25  10   1 ... 52
.
.

Table "2"

depth
|
v   1   2   3   4   5 ...
0   0   1   0   0   0 ...  1
1   0   2   1   0   0 ...  3
2   0   4   5   1   0 ... 10
3   0   8  19   9   1 ... 37
4   0  16  65  55  14 ...
.
.

#endif

namespace {

using partition_count_t = unsigned long;

// Cache of the number of partitions existing of 'groups' groups when starting with 'top_groups'
// and adding 'depth' new elements.
std::array<std::array<partition_count_t, number_of_elements * (number_of_elements + 1) / 2>, number_of_elements> table3d;

// Returns a reference into the cache for a given top_groups, depth and groups.
partition_count_t& number_of_partitions(int top_groups, int depth, int groups)
{
  // The cache is compressed: we don't store the zeroes.
  // That is, the inner array stores the triangle of non-zero values for a given 'Table' (for a given top_groups)
  // and the table for 'top_groups' is shifted top_groups - 1 to the left.
  return table3d[top_groups - 1][depth * (depth + 1) / 2 + groups - top_groups];
}

int table(int top_groups, int depth, int groups)
{
  assert(top_groups + depth <= number_of_elements);
  if (groups > depth + top_groups || groups < top_groups)
    return 0;
  partition_count_t& te = number_of_partitions(top_groups, depth, groups);
  if (te == 0)
  {
    if (depth == 0)
      te = (groups == top_groups) ? 1 : 0;
    else if (depth + top_groups == groups)
      te = 1;
    else
      te = table(top_groups, depth - 1, groups - 1) + groups * table(top_groups, depth - 1, groups);
  }
  return te;
}

partition_count_t number_of_partitions(int top_groups, int depth)
{
  partition_count_t sum = 0;
  for (int groups = top_groups; groups <= number_of_elements; ++groups)
  {
    partition_count_t term = table(top_groups, depth, groups);
    if (term == 0)
      break;
    sum += term;
  }
  return sum;
}

// Print the table 'top_groups'.
void print_table(int top_groups)
{
  std::cout << "  ";
  for (int groups = 1; groups <= number_of_elements; ++groups)
  {
    std::cout << std::setw(8) << groups;
  }
  std::cout << '\n';
  for (int depth = 0; depth <= number_of_elements - top_groups; ++depth)
  {
    std::cout << std::setw(2) << depth;
    for (int groups = 1; groups <= number_of_elements; ++groups)
    {
      int v = table(top_groups, depth, groups);
      std::cout << std::setw(8) << v;
    }
    std::cout << " = " << number_of_partitions(top_groups, depth) << '\n';
  }
  std::cout << '\n';
}

} // namespace

//static
utils::RandomNumber Partition::s_random_number;

//static
Partition Partition::random()
{
  Partition top(Group(Element('A')));
  int top_groups = 1;   // The current_root has 1 group.
  int top_elements = 1; // The current_root has 1 element.
  while (top_elements < number_of_elements)
  {
    Element const new_element('A' + top_elements);
    int depth = number_of_elements - top_elements;
    partition_count_t existing_group = number_of_partitions(top_groups, depth - 1);
    partition_count_t new_group = number_of_partitions(top_groups + 1, depth - 1);
    partition_count_t total = top_groups * existing_group + new_group;

#if 0
    std::cout << "top = " << top << '\n';
    std::cout << "top_groups = " << top_groups << '\n';
    std::cout << "top_elements = " << top_elements << '\n';
    std::cout << "depth = " << depth << '\n';
    std::cout << "existing_group = " << existing_group << '\n';
    std::cout << "new_group = " << new_group << '\n';
    std::cout << "total = " << total << '\n';
#endif

    std::uniform_int_distribution<partition_count_t> distr{0, total - 1};
    partition_count_t n = s_random_number.generate(distr);
//    std::cout << "random number = " << n << '\n';
    if (n >= top_groups * existing_group)
    {
      // Add new element to new group.
      top.add_to(GroupIndex{top_groups}, new_element);
      ++top_groups;
    }
    else
    {
      // Add new element to existing group.
      top.add_to(GroupIndex{static_cast<int>(n / existing_group)}, new_element);
    }
    ++top_elements;
  }
  return top;
}

Score Partition::find_local_maximum()
{
  Score current_score = score();
  //std::cout << rp << " : " << initial_partition << std::endl;
  constexpr int number_of_algorithms = 2;
  int no_improvement_count = 0;
  do
  {
    for (int algorithm = 0; algorithm < number_of_algorithms; ++algorithm)
    {
//        std::cout << "Algorithm " << algorithm << ":\n";
      Score last_score;

      // Anticipate that the current algorithm will fail to find any improvement.
      ++no_improvement_count;
      // Run algorithm 'algorithm'.
      do
      {
//          std::cout << *this << " = " << current_score << '\n';
        last_score = current_score;
        for (auto neighboring_partition_iterator =
            algorithm == 0 ? begin<PartitionIteratorWholeGroup>() : begin<PartitionIteratorSingleElement>() ;
             neighboring_partition_iterator != end();
             ++neighboring_partition_iterator)
        {
          Partition neighboring_partition = *neighboring_partition_iterator;
          Score neighboring_score = neighboring_partition.score();
          if (neighboring_score > current_score)
          {
            m_groups = neighboring_partition.m_groups;
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
