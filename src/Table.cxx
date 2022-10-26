#include "sys.h"
#include "Partition.h"
#include "utils/RandomNumber.h"
#include <iostream>
#include <iomanip>
#include <array>
#include "debug.h"

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

int main()
{
  Debug(NAMESPACE_DEBUG::init());
  Dout(dc::notice, "Entering main()");

  utils::RandomNumber rn;

  for (int j = 0; j < 100000000; ++j)
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
      partition_count_t n = rn.generate(distr);
  //    std::cout << "random number = " << n << '\n';
      if (n >= top_groups * existing_group)
      {
        // Add new element to new group.
        top.add_to(new_element, GroupIndex{top_groups});
        ++top_groups;
      }
      else
      {
        // Add new element to existing group.
        top.add_to(new_element, GroupIndex{static_cast<int>(n / existing_group)});
      }
      ++top_elements;
    }
    std::cout << top << '\n';
  }

#if 0 // Printing tables.
  for (int top_groups = 1; top_groups <= 4; ++top_groups)
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
#endif
}
