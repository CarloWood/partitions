#include "sys.h"
#include "Partition.h"
#include "PartitionIteratorSingleElement.h"
#include "PartitionIteratorWholeSet.h"
#include "PartitionIteratorBruteForce.h"
#include "PartitionIteratorScatter.h"
#include <cassert>
#include <iostream>
#include <iomanip>

Partition::Partition()
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
  for (ElementIndex e = Element::ibegin(); e != Element::iend(); ++e)
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

Score Partition::score() const
{
  Score sum;
  for (SetIndex g = m_sets.ibegin(); g != m_sets.iend(); ++g)
  {
    if (m_sets[g].empty())
      break;
    sum += m_sets[g].score();
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
v   1   2   3   4   5 ...     <-- sets (g)
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

// Cache of the number of partitions existing of 'sets' sets when starting with 'top_sets'
// and adding 'depth' new elements.
std::array<std::array<partition_count_t, number_of_elements * (number_of_elements + 1) / 2>, number_of_elements> table3d;

// Returns a reference into the cache for a given top_sets, depth and sets.
partition_count_t& number_of_partitions(int top_sets, int depth, int sets)
{
  // The cache is compressed: we don't store the zeroes.
  // That is, the inner array stores the triangle of non-zero values for a given 'Table' (for a given top_sets)
  // and the table for 'top_sets' is shifted top_sets - 1 to the left.
  return table3d[top_sets - 1][depth * (depth + 1) / 2 + sets - top_sets];
}

int table(int top_sets, int depth, int sets)
{
  assert(top_sets + depth <= number_of_elements);
  if (sets > depth + top_sets || sets < top_sets)
    return 0;
  partition_count_t& te = number_of_partitions(top_sets, depth, sets);
  if (te == 0)
  {
    if (depth == 0)
      te = (sets == top_sets) ? 1 : 0;
    else if (depth + top_sets == sets)
      te = 1;
    else
      te = table(top_sets, depth - 1, sets - 1) + sets * table(top_sets, depth - 1, sets);
  }
  return te;
}

partition_count_t number_of_partitions(int top_sets, int depth)
{
  partition_count_t sum = 0;
  for (int sets = top_sets; sets <= number_of_elements; ++sets)
  {
    partition_count_t term = table(top_sets, depth, sets);
    if (term == 0)
      break;
    sum += term;
  }
  return sum;
}

// Print the table 'top_sets'.
void print_table(int top_sets)
{
  std::cout << "  ";
  for (int sets = 1; sets <= number_of_elements; ++sets)
  {
    std::cout << std::setw(8) << sets;
  }
  std::cout << '\n';
  for (int depth = 0; depth <= number_of_elements - top_sets; ++depth)
  {
    std::cout << std::setw(2) << depth;
    for (int sets = 1; sets <= number_of_elements; ++sets)
    {
      int v = table(top_sets, depth, sets);
      std::cout << std::setw(8) << v;
    }
    std::cout << " = " << number_of_partitions(top_sets, depth) << '\n';
  }
  std::cout << '\n';
}

} // namespace

//static
utils::RandomNumber Partition::s_random_number;

//static
Partition Partition::random()
{
  Partition top(Set(Element('A')));
  int top_sets = 1;   // The current_root has 1 set.
  int top_elements = 1; // The current_root has 1 element.
  while (top_elements < number_of_elements)
  {
    Element const new_element('A' + top_elements);
    int depth = number_of_elements - top_elements;
    partition_count_t existing_set = number_of_partitions(top_sets, depth - 1);
    partition_count_t new_set = number_of_partitions(top_sets + 1, depth - 1);
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
            algorithm == 0 ? begin<PartitionIteratorWholeSet>() : begin<PartitionIteratorSingleElement>() ;
             neighboring_partition_iterator != end();
             ++neighboring_partition_iterator)
        {
          Partition neighboring_partition = *neighboring_partition_iterator;
          Score neighboring_score = neighboring_partition.score();
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
