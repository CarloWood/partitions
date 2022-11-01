#ifndef PARTITION_H
#define PARTITION_H

#include "Element.h"
#include "Set.h"
#include "utils/Vector.h"
#include "utils/RandomNumber.h"
#include <iosfwd>
#include <algorithm>

class PartitionIterator;
class PartitionIteratorBruteForce;
class PartitionIteratorScatter;
class PartitionTask;

class Partition
{
 private:
  friend class PartitionIterator;
  utils::Vector<Set, SetIndex> m_sets;

 public:
  Partition(int number_of_elements);
  Partition(int number_of_elements, Set set) : m_sets(number_of_elements)
  {
    auto set_iter = m_sets.begin();
    *set_iter = set;
    while (++set_iter != m_sets.end())
      set_iter->clear();
  }
  // Only used by PartitionIteratorBruteForce.
  Partition(ElementIndex number_of_elements, utils::Array<Set, max_number_of_elements, SetIndex> const& sets) :
    m_sets(sets.begin(), sets.begin() + number_of_elements())
  {
    sort();
  }

  void print_sets() const;
  void print_on(std::ostream& os) const;

  Set set(SetIndex set_index) const
  {
    return m_sets[set_index];
  }

  ElementIndex element_ibegin() const { return {utils::bitset::index_begin}; }
  ElementIndex element_iend() const { return ElementIndexPOD{static_cast<int8_t>(m_sets.size())}; }

  SetIndex set_ibegin() const { return m_sets.ibegin(); }
  SetIndex set_iend() const { return m_sets.iend(); }

  template<typename Algorithm>
  PartitionIterator begin() const;

  PartitionIterator end() const;

  PartitionIteratorScatter sbegin(PartitionTask const& partition_task);
  PartitionIteratorScatter send();

  SetIndex number_of_sets() const;
  bool is_alone(Element element) const;
  SetIndex set_of(Element element) const;
  SetIndex first_empty_set() const;
  Score score(PartitionTask const& partition_task) const;

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

  Score find_local_maximum(PartitionTask const& partition_task);

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
