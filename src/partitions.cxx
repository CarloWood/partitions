#include "sys.h"
#include "ElementPair.h"
#include "PartitionIteratorBase.h"
#include "PartitionIterator.h"
#include "PartitionIteratorBruteForce.h"
#include "utils/MultiLoop.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <random>
#include <chrono>
#include <memory>
#include <set>

std::array<Score, 8> g_possible_scores = {
  negative_inf,
  -100,
  -5,
  -1,
  1,
  5,
  100,
  positive_inf
};

std::array<int, 8> frequency = {
  1,
  number_of_elements,
  number_of_elements,
  number_of_elements,
  number_of_elements,
  number_of_elements,
  number_of_elements,
  1
};

#if 0
Element const A{'A'};
Element const B{'B'};
Element const C{'C'};
Element const D{'D'};
Element const E{'E'};
Element const F{'F'};
Element const G{'G'};
Element const H{'H'};
Element const I{'I'};
Element const J{'J'};
Element const K{'K'};
Element const L{'L'};
Element const M{'M'};
Element const N{'N'};
Element const O{'O'};
Element const P{'P'};
#endif

int main()
{
  int n = 0;
  for (auto bi = Partition::bbegin(); !bi.is_end(); ++bi)
  {
    Partition p = *bi;
    std::cout << std::setw(4) << n << " : ";
    p.print_groups();
    std::cout << '\n';
    ++n;
  }
  return 0;

  int frequency_sum = 0;
  for (int f : frequency)
    frequency_sum += f;
  std::vector<int> possible_scores_index_from_distribution;
  int j = 0;
  int next_count = frequency[0];
  for (int i = 0; i < frequency_sum; ++i)
  {
    if (i == next_count)
    {
      ++j;
      next_count += frequency[j];
    }
    possible_scores_index_from_distribution.push_back(j);
  }

  std::random_device rd;
  std::mt19937::result_type seed = rd() ^ (
          (std::mt19937::result_type)
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch()
              ).count() +
          (std::mt19937::result_type)
          std::chrono::duration_cast<std::chrono::microseconds>(
              std::chrono::high_resolution_clock::now().time_since_epoch()
              ).count() );
  std::cout << "seed = " << std::hex << seed << std::dec << std::endl;
  std::mt19937 generator(seed);
  std::uniform_int_distribution<int> distribution(0, frequency_sum - 1);

  char const* sep = "    ";
  std::cout << "    ";
  for (ElementIndex i1 = Element::ibegin(); i1 != Element::iend(); ++i1)
    std::cout << sep << Element(i1);
  std::cout << '\n';
  std::string indent("    ");
  for (ElementIndex i1 = Element::ibegin(); i1 != Element::iend(); ++i1)
  {
    std::cout << "    " << Element(i1) << indent;
    char const* prefix = "";
    for (ElementIndex i2 = i1 + 1; i2 != Element::iend(); ++i2)
    {
      ElementPair ep(i1, i2);
      elements_t::mask_type score_index = ep.get_pair().to_ulong();
      if (g_scores[score_index].is_zero())
        g_scores[score_index] = g_possible_scores[possible_scores_index_from_distribution[distribution(generator)]];

      std::cout << std::setw(5) << ep.score();
      //std::cout << ep << " : " << score(ep) << '\n';
    }
    std::cout << '\n';
    indent += sep;
    indent += ' ';
  }

  Score max_score(negative_inf);
  std::map<Partition, int> results;
  for (auto bi = Partition::bbegin(); !bi.is_end(); ++bi)
  {
    Partition initial_partition = *bi;
    Partition current_partition = initial_partition;
    Score current_score = current_partition.score();
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
//          std::cout << current_partition << " = " << current_score << '\n';
          last_score = current_score;
          for (auto neighboring_partition_iterator =
              algorithm == 0 ? current_partition.begin<PartitionIteratorWholeGroup>() : current_partition.begin<PartitionIteratorSingleElement>() ;
               neighboring_partition_iterator != current_partition.end();
               ++neighboring_partition_iterator)
          {
            Partition neighboring_partition = *neighboring_partition_iterator;
            Score neighboring_score = neighboring_partition.score();
            if (neighboring_score > current_score)
            {
              current_partition = neighboring_partition;
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
    auto res = results.try_emplace(current_partition, 0);
    res.first->second += 1;
    if (current_score > max_score)
      max_score = current_score;
  }
  for (auto&& r : results)
    std::cout << r.first.score() << " : " << r.first << " : " << r.second << '\n';
  std::cout << "Maximum score = " << max_score << '\n';
}
