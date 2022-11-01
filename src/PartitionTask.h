#pragma once

#include "PartitionIteratorBruteForce.h"
#include <map>

class PartitionTask
{
 public:
  using partition_count_t = unsigned long;

 private:
  //FIXME: can't this be a temporary?
  static std::array<std::array<partition_count_t, max_number_of_elements * (max_number_of_elements + 1) / 2>, max_number_of_elements> s_table3d;

  int m_number_of_elements;
  std::map<Set, Score> m_set23_to_score;        // Initialized by initialize_set23_to_score.
  bool m_set23_to_score_initialized{false};     // Set to true when m_set23_to_score is initialized.
  std::vector<Score> m_scores;

 public:
  PartitionTask(int number_of_elements) :
    m_number_of_elements(number_of_elements), m_scores(64 * (number_of_elements - 1) + number_of_elements) { }

  static partition_count_t& number_of_partitions(int top_sets, int depth, int sets);
  static int table(int top_sets, int depth, int sets);
  static partition_count_t number_of_partitions(int top_sets, int depth);
  static void print_table(int top_sets);

  ElementIndex ibegin() const
  {
    return ElementIndexPOD{0};
  }

  ElementIndex iend() const
  {
    return ElementIndexPOD{static_cast<int8_t>(m_number_of_elements)};
  }

  PartitionIteratorBruteForce bbegin()
  {
    return {m_number_of_elements};
  }

  PartitionIteratorBruteForce bend()
  {
    return {};
  }

  void initialize_set23_to_score();
  void initialize_scores();

  void set_score(int score_index, Score score)
  {
    m_scores[score_index] = score;
  }

  Score const& score(int score_index) const
  {
    return m_scores[score_index];
  }

  void print_on(std::ostream& os) const;
};
