#include "sys.h"
#include "ElementPair.h"
#include "Partition.h"
#include "PartitionIteratorScatter.h"
#include "GroupIteratorScatter.h"
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
Element const G{'G'};
Element const H{'H'};
Element const I{'I'};
Element const J{'J'};
#if 0
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

  utils::RandomNumber random_number(0x5ebf860a924ad);
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
      int score_index = ep.score_index();
      if (g_scores[score_index].is_zero())
        g_scores[score_index] = g_possible_scores[possible_scores_index_from_distribution[random_number.generate(distribution)]];

      std::cout << std::setw(5) << ep.score();
      //std::cout << ep << " : " << score(ep) << '\n';
    }
    std::cout << '\n';
    indent += sep;
    indent += ' ';
  }

  Score max_score(negative_inf);
  std::map<Partition, int> results;
  for (int rp = 0; rp < 1000; ++rp)
  {
    Partition partition = Partition::random();
    Score score = partition.find_local_maximum();
    //std::cout << score << " : " << partition << '\n';
    int count = 0;
    for (PartitionIteratorScatter scatter = partition.sbegin(); !scatter.is_end(); ++scatter)
    {
      Partition partition2 = *scatter;
      Score score2 = partition2.find_local_maximum();
      if (score2 > score)
      {
        partition = partition2;
        score = score2;
      }
      if (++count == 100)
        break;
    }
    auto res = results.try_emplace(partition, 0);
    res.first->second += 1;
    if (score > max_score)
      max_score = score;
  }
  std::multimap<Score, std::map<Partition, int>::iterator> score_to_partition;
  for (auto ri = results.begin(); ri != results.end(); ++ri)
    score_to_partition.insert(std::make_pair(ri->first.score(), ri));
  for (auto spi = score_to_partition.begin(); spi != score_to_partition.end(); ++spi)
    std::cout << spi->first << " : " << spi->second->first << " : " << spi->second->second << '\n';
  std::cout << "Maximum score = " << max_score << '\n';
}
