#include "sys.h"
#include "ElementPair.h"
#include "Partition.h"
#include "PartitionIteratorExplode.h"
#include "PartitionTask.h"
#include "PairTripletIteratorExplode.h"
#include "utils/MultiLoop.h"
#include "utils/debug_ostream_operators.h"
#include "utils/RandomNumber.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <random>
#include <chrono>
#include <memory>
#include <set>
#include <map>
#include <cmath>
#include "debug.h"

constexpr int8_t number_of_elements = 32;
constexpr int8_t max_number_of_sets = 10;

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

Element const A{'A'};
Element const B{'B'};
Element const C{'C'};
Element const D{'D'};
Element const E{'E'};
Element const F{'F'};
#if 0
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
  Debug(NAMESPACE_DEBUG::init());
  Dout(dc::notice, "Entering main()");

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

  utils::RandomNumber random_number(0x5ec5853653bbd);
  std::uniform_int_distribution<int> distribution(0, frequency_sum - 1);

  PartitionTask partition_task(number_of_elements, max_number_of_sets);

  char const* sep = "    ";
  std::cout << "    ";
  for (ElementIndex i1 = partition_task.ibegin(); i1 != partition_task.iend(); ++i1)
    std::cout << sep << Element(i1);
  std::cout << '\n';
  std::string indent("    ");
  for (ElementIndex i1 = partition_task.ibegin(); i1 != partition_task.iend(); ++i1)
  {
    std::cout << "    " << Element(i1) << indent;
    char const* prefix = "";
    for (ElementIndex i2 = i1 + 1; i2 != partition_task.iend(); ++i2)
    {
      ElementPair ep(i1, i2);
      int score_index = ep.score_index();
      if (partition_task.score(score_index).is_zero())
        partition_task.set_score(score_index, g_possible_scores[possible_scores_index_from_distribution[random_number.generate(distribution)]]);

      std::cout << std::setw(5) << ep.score(partition_task);
      //std::cout << ep << " : " << score(ep) << '\n';
    }
    std::cout << '\n';
    indent += sep;
    indent += ' ';
  }

  partition_task.initialize_set23_to_score();

#if 0
  for (PartitionIteratorBruteForce bi = partition_task.bbegin(partition_task); !bi.is_end(); ++bi)
    Dout(dc::notice, *bi);
#endif

  Score max_score(negative_inf);
  std::map<Partition, int> results;
  int best_rp = 0;
  int best_count = 0;
  for (int rp = 0; rp < 1000; ++rp)
  {
    Partition partition = partition_task.random();
    Score score = partition.find_local_maximum(partition_task);
    int count = 0;
    for (PartitionIteratorExplode explode = partition.sbegin(partition_task); !explode.is_end(); ++explode)
    {
      Partition partition2 = explode.get_partition(partition_task);
      Score score2 = partition2.find_local_maximum(partition_task);
      if (score2 > score)
      {
        partition = partition2;
        score = score2;
      }
      if (++count == PartitionIteratorExplode::total_loop_count_limit)
        break;
    }
    auto res = results.try_emplace(partition, 0);
    res.first->second += 1;
    if (score > max_score)
    {
      max_score = score;
      best_rp = rp;
      best_count = 1;
    }
    else if (score.unchanged(max_score))
      ++best_count;
    // x^y = e^(ln(x)*y)
    if (rp > std::max(20, 2 * best_rp) && std::exp(rp * std::log(static_cast<double>(rp - best_count) / rp)) < 0.01)
    {
      Dout(dc::notice, "rp = " << rp);
      break;
    }
  }
  std::multimap<Score, std::map<Partition, int>::iterator> score_to_partition;
  for (auto ri = results.begin(); ri != results.end(); ++ri)
    score_to_partition.insert(std::make_pair(ri->first.score(partition_task), ri));
  for (auto spi = score_to_partition.begin(); spi != score_to_partition.end(); ++spi)
    std::cout << spi->first << " : " << spi->second->first << " : " << spi->second->second << '\n';
  std::cout << "Maximum score = " << max_score << '\n';
}
