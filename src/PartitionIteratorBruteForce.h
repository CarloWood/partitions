#pragma once

#include "Partition.h"
#include "utils/MultiLoop.h"
#include <array>

class PartitionIteratorBruteForce
{
 private:
  MultiLoop m_multiloop{number_of_elements};
  std::array<int, number_of_elements> m_loop_value_count;       // The number of loop counter with a value equal to the index.

 public:
  // Create an end iterator.
  PartitionIteratorBruteForce() : m_multiloop(number_of_elements, 1) { }
  // Create the begin iterator.
  PartitionIteratorBruteForce(int);

  PartitionIteratorBruteForce& operator++();

  bool is_end() const
  {
    return m_multiloop.finished();
  }

  Partition operator*() const;
};
