#include "sys.h"
#include "PartitionTask.h"
#include "ElementPair.h"
#include <iomanip>

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

// Cache of the number of partitions existing of 'sets' sets when starting with 'top_sets'
// and adding 'depth' new elements.
//static
std::array<std::array<PartitionTask::partition_count_t, max_number_of_elements * (max_number_of_elements + 1) / 2>, max_number_of_elements> PartitionTask::s_table3d;

// Returns a reference into the cache for a given top_sets, depth and sets.
//static
PartitionTask::partition_count_t& PartitionTask::number_of_partitions(int top_sets, int depth, int sets)
{
  // The cache is compressed: we don't store the zeroes.
  // That is, the inner array stores the triangle of non-zero values for a given 'Table' (for a given top_sets)
  // and the table for 'top_sets' is shifted top_sets - 1 to the left.
  return s_table3d[top_sets - 1][depth * (depth + 1) / 2 + sets - top_sets];
}

//static
int PartitionTask::table(int top_sets, int depth, int sets)
{
  assert(top_sets + depth <= max_number_of_elements);
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

//static
PartitionTask::partition_count_t PartitionTask::number_of_partitions(int top_sets, int depth)
{
  partition_count_t sum = 0;
  for (int sets = top_sets; sets <= max_number_of_elements; ++sets)
  {
    partition_count_t term = table(top_sets, depth, sets);
    if (term == 0)
      break;
    sum += term;
  }
  return sum;
}

// Print the table 'top_sets'.
//static
void PartitionTask::print_table(int top_sets)
{
  std::cout << "  ";
  for (int sets = 1; sets <= max_number_of_elements; ++sets)
  {
    std::cout << std::setw(8) << sets;
  }
  std::cout << '\n';
  for (int depth = 0; depth <= max_number_of_elements - top_sets; ++depth)
  {
    std::cout << std::setw(2) << depth;
    for (int sets = 1; sets <= max_number_of_elements; ++sets)
    {
      int v = table(top_sets, depth, sets);
      std::cout << std::setw(8) << v;
    }
    std::cout << " = " << number_of_partitions(top_sets, depth) << '\n';
  }
  std::cout << '\n';
}

void PartitionTask::initialize_set23_to_score()
{
  // Don't call initialize_set23_to_score() twice.
  ASSERT(!m_set23_to_score_initialized);
  m_set23_to_score_initialized = true;
  for (ElementIndex i1 = ibegin(); i1 != iend(); ++i1)
  {
    for (ElementIndex i2 = i1 + 1; i2 != iend(); ++i2)
    {
      ElementPair ep(i1, i2);
      int score_index = ep.score_index();
      Score const score = m_scores[score_index];
      Score zero;
      //Dout(dc::notice, "ep = " << ep << "; score = " << score << "; zero < score = " << std::boolalpha << (zero < score));
      if (zero < score)
        m_set23_to_score[Set(Element{i1}|Element{i2})] = score;
      for (ElementIndex i3 = i2 + 1; i3 != iend(); ++i3)
      {
        ElementPair ep13(i1, i3);
        ElementPair ep23(i2, i3);
        int score_index13 = ep13.score_index();
        int score_index23 = ep23.score_index();
        Score score13 = m_scores[score_index13];
        Score score23 = m_scores[score_index23];
        Score score3 = score;
        score3 += score13;
        score3 += score23;
        if (zero < score3)
          m_set23_to_score[Set(Element{i1}|Element{i2}|Element{i3})] = score3;
      }
    }
  }
}

void PartitionTask::print_on(std::ostream& os) const
{
  os << '{';
  os << "m_number_of_elements:" << m_number_of_elements <<
      ", m_set23_to_score:" << m_set23_to_score <<
      ", m_set23_to_score_initialized:" << std::boolalpha << m_set23_to_score_initialized <<
      ", m_scores:" << m_scores;
  os << '}';
}
