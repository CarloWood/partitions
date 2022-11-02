#pragma once

#include "Partition.h"
#include "utils/MultiLoop.h"
#include <array>

class PartitionIteratorBruteForce
{
 private:
  MultiLoop m_multiloop;
  std::vector<int> m_loop_value_count;       // The number of loop counter with a value equal to the index.

 public:
  // Create an end iterator.
  PartitionIteratorBruteForce() : m_multiloop(1, 1) { }
  // Create the begin iterator.
  PartitionIteratorBruteForce(int8_t number_of_elements);

  PartitionIteratorBruteForce& operator++();

  bool is_end() const
  {
    return m_multiloop.finished();
  }

  Partition operator*() const;
};
