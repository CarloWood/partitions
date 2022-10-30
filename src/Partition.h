#ifndef PARTITION_H
#define PARTITION_H

#include "Element.h"
#include "Set.h"
#include "utils/Array.h"
#include "utils/RandomNumber.h"
#include <iosfwd>
#include <algorithm>

class PartitionIterator;
class PartitionIteratorBruteForce;
class PartitionIteratorScatter;

class Partition
{
 private:
  friend class PartitionIterator;
  utils::Array<Set, number_of_elements, SetIndex> m_sets;

 public:
  Partition();

  template<ConceptSet... GROUP>
  Partition(GROUP... sets) : m_sets{sets...}
  {
    for (SetIndex gi{sizeof...(sets)}; gi != m_sets.iend(); ++gi)
      m_sets[gi] = Set{elements_t{elements_t::mask_type{0}}};
    sort();
  }

  Partition(utils::Array<Set, number_of_elements, SetIndex> const& sets) : m_sets(sets) { sort(); }

  void print_sets() const;
  void print_on(std::ostream& os) const;

  Set set(SetIndex set_index) const
  {
    return m_sets[set_index];
  }

  auto gbegin() const { return m_sets.ibegin(); }
  auto gend() const { return m_sets.iend(); }

  template<typename Algorithm>
  PartitionIterator begin() const;

  PartitionIterator end() const;

  static PartitionIteratorBruteForce bbegin();
  static PartitionIteratorBruteForce bend();

  PartitionIteratorScatter sbegin();
  PartitionIteratorScatter send();

  static utils::RandomNumber s_random_number;
  static Partition random();

  SetIndex number_of_sets() const;
  bool is_alone(Element element) const;
  SetIndex set_of(Element element) const;
  SetIndex first_empty_set() const;
  Score score() const;

  void sort()
  {
    std::sort(m_sets.rbegin(), m_sets.rend());
  }

  void add_to(SetIndex set_index, Set set)
  {
    m_sets[set_index].add(set);
  }

  void remove_from(SetIndex set_index, Set set)
  {
    m_sets[set_index].remove(set);
  }

  Score find_local_maximum();

  friend bool operator<(Partition const& lhs, Partition const& rhs)
  {
    return lhs.m_sets < rhs.m_sets;
  }

  friend std::ostream& operator<<(std::ostream& os, Partition const& partition);
};

#endif // PARTITION_H

#ifndef PARTITION_ITERATOR_H
#include "PartitionIterator.h"
#endif

#ifndef PARTITION_H_definitions
#define PARTITION_H_definitions

template<typename Algorithm>
PartitionIterator Partition::begin() const
{
  return {*this, std::make_unique<Algorithm>(*this)};
}

#endif // PARTITION_H_definitions
